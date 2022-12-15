//
// Created by root on 12/13/22.
//

#ifndef LAB1_RW_LOCK_H
#define LAB1_RW_LOCK_H

#define READER_FIRST 0
#define WRITER_FIRST 1

typedef struct reader_writer_lock *rw_lock;

rw_lock new_rw_lock(int type) ;

void r_lock(rw_lock l);
void r_unlock(rw_lock l);
void w_lock(rw_lock l);
void w_unlock(rw_lock l);

void destroy_rw_lock(rw_lock l);

#endif //LAB1_RW_LOCK_H
