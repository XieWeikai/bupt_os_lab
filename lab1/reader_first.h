//
// Created by root on 12/13/22.
//

#ifndef LAB1_READER_FIRST_H
#define LAB1_READER_FIRST_H

typedef struct reader_first_rw_lock *rf_rw_lock;

rf_rw_lock new_rf_rw_lock();

void destroy_rf_rw_lock(rf_rw_lock);

void rf_r_lock(rf_rw_lock lock);

void rf_r_unlock(rf_rw_lock lock);

void rf_w_lock(rf_rw_lock lock);

void rf_w_unlock(rf_rw_lock lock);

#endif //LAB1_READER_FIRST_H
