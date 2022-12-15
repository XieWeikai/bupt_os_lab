//
// Created by root on 12/13/22.
//
#include <stdlib.h>
#include <semaphore.h>

#include "writer_first.h"

typedef struct writer_first_rw_lock{
    int reader_cnt,writer_cnt;  // number of reader and writer
    sem_t r_cnt_lock,w_cnt_lock; // lock for reader_cnt and writer_cnt
    sem_t no_writer;             // reader can use resource only when there is no writer
    sem_t resource;              // lock resource
}writer_first_rw_lock;


wf_rw_lock new_wf_rw_lock() {
    wf_rw_lock t = malloc(sizeof(writer_first_rw_lock));
    if(t == NULL)
        return NULL;

    t->reader_cnt =  t->writer_cnt = 0;
    sem_init(&t->r_cnt_lock,0,1);
    sem_init(&t->w_cnt_lock,0,1);
    sem_init(&t->no_writer,0,1);
    sem_init(&t->resource,0,1);

    return t;
}

void destroy_wf_rw_lock(wf_rw_lock l) {
    sem_destroy(&l->r_cnt_lock);
    sem_destroy(&l->w_cnt_lock);
    sem_destroy(&l->no_writer);
    sem_destroy(&l->resource);
}

void wf_r_lock(wf_rw_lock l) {
    sem_wait(&l->no_writer);   // only when there is no writer

    sem_wait(&l->r_cnt_lock);
    l->reader_cnt ++;
    if(l->reader_cnt == 1) // first reader
        sem_wait(&l->resource);
    sem_post(&l->r_cnt_lock);

    sem_post(&l->no_writer);
}

void wf_r_unlock(wf_rw_lock l) {
    sem_wait(&l->r_cnt_lock);
    l->reader_cnt --;
    if(l->reader_cnt == 0) // first reader
        sem_post(&l->resource);
    sem_post(&l->r_cnt_lock);
}

void wf_w_lock(wf_rw_lock l) {
    sem_wait(&l->w_cnt_lock);
    l->writer_cnt ++;
    if(l->writer_cnt == 1)   // first writer
        sem_wait(&l->no_writer);  // block reader out
    sem_post(&l->w_cnt_lock);

    sem_wait(&l->resource);
}

void wf_w_unlock(wf_rw_lock l) {
    sem_post(&l->resource);

    sem_wait(&l->w_cnt_lock);
    l->writer_cnt --;
    if(l->writer_cnt == 0)
        sem_post(&l->no_writer);
    sem_post(&l->w_cnt_lock);
}

