//
// Created by root on 12/13/22.
//
#include <stdlib.h>

#include "reader_first.h"
#include "writer_first.h"
#include "rw_lock.h"

typedef struct reader_writer_lock{
    void *lock;

    // rw_lock interface
    void (*r_lock)(void *);
    void (*r_unlock)(void *);
    void (*w_lock)(void *);
    void (*w_unlock)(void *);

    void (*destroy)(void *);
}reader_writer_lock;

rw_lock new_rw_lock(int type) {
    rw_lock t = malloc(sizeof(reader_writer_lock));
    if(t == NULL)
        return NULL;

    if(type == READER_FIRST) {
        t->lock = new_rf_rw_lock();
        if(t->lock == NULL)
            return NULL;
        t->r_lock = (void (*)(void *)) rf_r_lock;
        t->r_unlock = (void (*)(void *)) rf_r_unlock;
        t->w_lock = (void (*)(void *)) rf_w_lock;
        t->w_unlock = (void (*)(void *)) rf_w_unlock;

        t->destroy = (void (*)(void *)) destroy_rf_rw_lock;
    } else {  // WRITER_FIRST
        t->lock = new_wf_rw_lock();
        if(t->lock == NULL)
            return NULL;
        t->r_lock = (void (*)(void *)) wf_r_lock;
        t->r_unlock = (void (*)(void *)) wf_r_unlock;
        t->w_lock = (void (*)(void *)) wf_w_lock;
        t->w_unlock = (void (*)(void *)) wf_w_unlock;

        t->destroy = (void (*)(void *)) destroy_wf_rw_lock;
    }

    return t;
}

void r_lock(rw_lock l) {
    l->r_lock(l->lock);
}

void r_unlock(rw_lock l) {
    l->r_unlock(l->lock);
}

void w_lock(rw_lock l) {
    l->w_lock(l->lock);
}

void w_unlock(rw_lock l) {
    l->w_unlock(l->lock);
}

void destroy_rw_lock(rw_lock l) {
    l->destroy(l->lock);
}
