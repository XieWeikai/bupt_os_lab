//
// Created by 谢卫凯 on 2022/12/14.
//
#include <stdio.h>
#include <stdlib.h>

#include "contiguous.h"

static block_node *create_block_node(void *start,size_t s){
    block_node *t = malloc(sizeof(block_node));
    if(t == NULL)
        return NULL;
    t->start = start;
    t->size = s;
    t->next = t->pre = NULL;

    return t;
}

static block_node *insert_block_node(block_list *bl,block_node *bn){
    block_node *next,*pre;
    for(next = bl->head->next; next != bl->tail && next->start <= bn->start; next = next->next) // find next node of bn
        ;
    pre = next->pre;
    bn->next = next;
    bn->pre = pre;
    pre->next = bn;
    next->pre = bn;

    return bn;
}

// find first blockt that can fit
static block_node *fist_fit(block_list *bl,size_t s){
    block_node *t;

    for(t = bl->head->next; t != bl->tail;t = t->next)
        if(t->size >= s + sizeof(size_t))  // add header's size
            return t;

    return NULL;  // not found
}

// find the smallest block that can fit
static block_node *best_fit(block_list *bl,size_t s){
    block_node *t,*best = NULL;

    for(t = bl->head->next; t != bl->tail;t = t->next)
        if(t->size >= s + sizeof(size_t))  // add header's size
            if(best == NULL || t->size < best->size)
                best = t;

    return best;
}

// find the largest block that can fit
static block_node *worst_fit(block_list *bl,size_t s){
    block_node *t,*worst = NULL;

    for(t = bl->head->next; t != bl->tail;t = t->next)
        if(t->size >= s + sizeof(size_t))  // add header's size
            if(worst == NULL || t->size > worst->size)
                worst = t;

    return worst;
}

static int init_block_list(block_list *bl) {
    bl->head = bl->tail = malloc(sizeof(block_node));
    if(bl->tail == NULL)
        return 0;  // failed

    bl->head->next = bl->tail;
    bl->tail->pre = bl->head;

    return 1;
}

mem_pool *create_mem_pool(size_t s,int type){
    mem_pool *t = malloc(sizeof(mem_pool));
    if(t == NULL)
        return NULL;
    t->data = malloc(s);
    if(t->data == NULL){
        free(t);
        return NULL;
    }

    if(!init_block_list(&t->free_mem)){
        free(t);
        return NULL;
    }

    insert_block_node(&t->free_mem, create_block_node(t->data,s));

    if(type == FIRST_FIT)
        t->find_block = fist_fit;

    if(type == BEST_FIT)
        t->find_block = best_fit;

    if(type == WORST_FIT)
        t->find_block = worst_fit;

    return t;
}

static void detach_block_node(block_node *bn){
    block_node *pre,*next;
    pre = bn->pre;
    next = bn->next;
    pre->next = bn->next;
    next->pre = bn->pre;
}

void *p_allocate(mem_pool *pool, size_t s) {
    block *b;

    block_node *t = pool->find_block(&pool->free_mem,s); // find a block
    if(t == NULL)
        return NULL; // failed to find a block that can fit

    b = t->start;
    b->size = s + sizeof(size_t);  // record size

    t->start = t->start + b->size;     // update start
    t->size -= b->size;                // update size

    if(t->size <= 0){
        detach_block_node(t);  // this block has no more space ,delete this block node
        free(t);
    }

    return b->data;
}

// merge two block node
// a->next == b
static block_node *merge(block_node *a,block_node *b){
    a->size += b->size;
    a->next = b->next;
    b->next->pre = a;
    free(b);
    return a;
}

void p_free(mem_pool *pool,void *data) {
    block *b;
    b = data - sizeof(size_t);

    block_node *t = insert_block_node(&pool->free_mem, create_block_node(b,b->size));

    block_node *pre,*next;
    pre = t->pre;
    next = t->next;

    if(pre != pool->free_mem.head && pre->start + pre->size == t->start)
        t = merge(pre,t);


    if(next != pool->free_mem.tail && t->start + t->size == next->start)
        merge(t,next);

}

int print_free_block(FILE *fp,mem_pool *pool) {
    int cnt = 0;
    block_node *t;
    fprintf(fp,"free blocks: ");
    for(t = pool->free_mem.head->next;t != pool->free_mem.tail;t = t->next) {
        fprintf(fp, "(%lu,%lu)  ", t->start - pool->data, t->start + t->size - pool->data);
        cnt ++;
    }

    fprintf(fp,"\nframent:%d\n\n",cnt);
    return cnt;
}

int get_fragments(mem_pool *pool){
    int cnt = 0;
    block_node *t;
    for(t = pool->free_mem.head->next;t != pool->free_mem.tail;t = t->next)
        cnt ++;
    return cnt;
}

void destroy_pool(mem_pool *pool) {
    block_node *t,*tn;

    for(t = pool->free_mem.head->next,tn = t->next;
                t != pool->free_mem.tail;t = tn,tn = tn->next)
        free(t);
    free(pool->free_mem.head);

    free(pool->data);
    free(pool);
}

