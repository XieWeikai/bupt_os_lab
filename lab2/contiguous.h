//
// Created by 谢卫凯 on 2022/12/14.
//

#ifndef LAB2_CONTIGUOUS_H
#define LAB2_CONTIGUOUS_H

#include <stdlib.h>

// 256 MB
#define POOL_SIZE (1 << 28)

#define FIRST_FIT 0
#define BEST_FIT 1
#define WORST_FIT 3


// 在空闲空间中分配的一块内存即为一个这个结构
// memory_pool中即为多个该结构
// data可变长
// 每次用户分配一块大小为size的空间时
// 实际上会分配一块大小为 size + sizeof(size_t) 的该结构
// 给用户返回data域
typedef struct block{
    size_t size;            // block size:sizeof(data) + sizeof(size_t)
    char data[];            // data
}block;

typedef struct block_node{
    void *start;     // start address
    size_t size;     // size of the block
    struct block_node *pre,*next;  // double link
}block_node;

typedef struct block_list{
    // point to the head node and the tail node
    // head node and tail node not used
    block_node *head,*tail;
}block_list;

typedef struct memory_pool{
    void *data;  // memory
    block_list free_mem ;  // free memory block list

    // configurable
    // can use first-fit best-fit worst-fit or other strategies
    block_node *(*find_block)(block_list *,size_t s);
}mem_pool;

mem_pool *create_mem_pool(size_t s,int type);

void destroy_pool(mem_pool *pool);

void *p_allocate(mem_pool *pool,size_t s);

void p_free(mem_pool *pool,void *);

int print_free_block(FILE *fp,mem_pool *pool);

int get_fragments(mem_pool *pool);

#endif //LAB2_CONTIGUOUS_H
