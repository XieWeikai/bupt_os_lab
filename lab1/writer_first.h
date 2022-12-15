//
// Created by root on 12/13/22.
//

#ifndef LAB1_WRITER_FIRST_H
#define LAB1_WRITER_FIRST_H

typedef struct writer_first_rw_lock *wf_rw_lock;

wf_rw_lock new_wf_rw_lock();

void destroy_wf_rw_lock(wf_rw_lock l);

void wf_r_lock(wf_rw_lock l);

void wf_r_unlock(wf_rw_lock l);

void wf_w_lock(wf_rw_lock l);

void wf_w_unlock(wf_rw_lock l);

#endif //LAB1_WRITER_FIRST_H
