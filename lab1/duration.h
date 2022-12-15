//
// Created by root on 12/14/22.
//

#ifndef LAB1_DURATION_H
#define LAB1_DURATION_H

typedef struct timer{
    struct timespec start;
}my_timer;

void new_timer(my_timer *t);
double ms_from_start(my_timer *t);

#endif //LAB1_DURATION_H
