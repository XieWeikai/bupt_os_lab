//
// Created by root on 12/13/22.
//
#include <stdlib.h>
#include <semaphore.h>

#include "reader_first.h"

typedef struct reader_first_rw_lock{
    int reader_cnt;    // number of reader
    sem_t cnt_lock;    // lock reader_cnt
    sem_t resource;    // lock resource
}reader_first_rw_lock;

rf_rw_lock new_rf_rw_lock(){
    rf_rw_lock t = malloc(sizeof(reader_first_rw_lock));

    if(t == NULL)
        return NULL; // fail to allocate memory

    t->reader_cnt = 0;
    sem_init(&t->cnt_lock,0,1);
    sem_init(&t->resource, 0, 1);

    return t;
}

void destroy_rf_rw_lock(rf_rw_lock t) {
    sem_destroy(&t->cnt_lock);
    sem_destroy(&t->resource);
    free(t);
}

void rf_r_lock(rf_rw_lock lock){
    sem_wait(&lock->cnt_lock); // lock reader_cnt
    lock->reader_cnt ++;           // reader + 1
    if(lock->reader_cnt == 1)      // this is the first reader
        sem_wait(&lock->resource);  // reader tries to occupy the resource
    sem_post(&lock->cnt_lock); // release reader_cnt
}

void rf_r_unlock(rf_rw_lock lock) {
    sem_wait(&lock->cnt_lock); // lock reader_cnt
    lock->reader_cnt --;
    if(lock->reader_cnt == 0)      // last reader
        sem_post(&lock->resource);  // release the resource so that writer can occupy
    sem_post(&lock->cnt_lock); // release reader_cnt
}

void rf_w_lock(rf_rw_lock lock) {
    sem_wait(&lock->resource);    // try to occupy the resource
}

void rf_w_unlock(rf_rw_lock lock) {
    sem_post(&lock->resource);
}

