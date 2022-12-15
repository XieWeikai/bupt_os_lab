//
// Created by root on 12/14/22.
//

#include <time.h>

#include "duration.h"

void new_timer(my_timer *t){
    clock_gettime(CLOCK_MONOTONIC_RAW,&t->start);
}

double ms_from_start(my_timer *t){
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC_RAW,&end);
    return (double)(end.tv_nsec - t->start.tv_nsec) / 1000000 + (double)(end.tv_sec - t->start.tv_sec) * 1000 ;
}
