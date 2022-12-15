//
// Created by 谢卫凯 on 2022/12/11.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "rw_lock.h"
#include "duration.h"

#define MILLI_SECOND 1000


rw_lock rwLock;

unsigned long writer_cnt = 0;  // count write times
double w_total_time = 0.0;  // ms
pthread_mutex_t wlock = PTHREAD_MUTEX_INITIALIZER;

unsigned long reader_cnt;      // count read times
double r_total_time = 0.0;  // ms
pthread_mutex_t rlock = PTHREAD_MUTEX_INITIALIZER;

long *resource;
// 一个刻意复杂化的写函数
// 若同步逻辑有错误
// 程序很容易崩溃
void write_resource(long num) {
    free(resource);
    resource = malloc(sizeof(long));
    *resource = num ;
}

long read_resource(){
    return *resource;
}

void *writer(void *arg){
    long t;
    my_timer timer;
    new_timer(&timer);
    double start,end;
    while(1) {
        w_lock(rwLock);

        start = ms_from_start(&timer);

        t = *resource;
        write_resource(t + 1);
        usleep(100 * MILLI_SECOND + (rand() % 2001) * MILLI_SECOND);

        end = ms_from_start(&timer);

        w_unlock(rwLock);

        pthread_mutex_lock(&wlock);
        writer_cnt ++;
        w_total_time += (end - start);
        pthread_mutex_unlock(&wlock);

        printf("writer(%d)     start:%.5f    end:%.5f \n",*(int *)arg,start / 1000 , end / 1000);

        // do nothing
        usleep((rand() % 2001) * MILLI_SECOND);
    }
}

void reader(void *arg){
    long t1,t2;
    my_timer timer;
    new_timer(&timer);
    double start,end;
    while(1){
        r_lock(rwLock);

        start = ms_from_start(&timer);
        t1 = read_resource();
        usleep(100 * MILLI_SECOND + (rand() % 2001) * MILLI_SECOND);
        t2 = read_resource();
        end = ms_from_start(&timer);

        if (t1 != t2)
            printf("ERROR: writer writes during reader is reading !!!!!!!!!!!! (%lu,%lu)\n",t1,t2);

        r_unlock(rwLock);

        pthread_mutex_lock(&rlock);
        reader_cnt ++;
        r_total_time += (end - start);
        pthread_mutex_unlock(&rlock);

        printf("reader(%d)  get:%lu   start:%.5f   end:%.5f \n",*(int *)arg,t1 ,start / 1000, end / 1000);

        // do nothing
        usleep((rand() % 2001) * MILLI_SECOND);
    }
}

void signal_handler(int signum){
    destroy_rw_lock(rwLock);
    printf("\n\n");
    printf("writer count:%lu writer time:%.5f \n",writer_cnt,w_total_time / 1000 ) ;
    printf("reader count:%lu reader time:%.5f \n",reader_cnt,r_total_time / 1000) ;
    fflush(stdout);
    exit(signum);
}

int main(int argc,char **argv){
    int type = READER_FIRST;

    if(argc > 1 && strcmp(argv[1],"writer") == 0)
        type = WRITER_FIRST;

    rwLock = new_rw_lock(type);
    resource = malloc(sizeof(long));

    srand(time(NULL));
    signal(SIGINT,signal_handler);
    signal(SIGTERM,signal_handler);

    int rid[] = {1,2,3,4,5,6,7,8};
    int wid[] = {1,2};
    pthread_t rids[8];
    pthread_t wids[2];

    for(int i = 0;i < 2;i++)
        pthread_create(wids + i, NULL, (void *(*)(void *)) writer, wid + i);

    for(int i = 0;i < 8;i++)
        pthread_create(rids + i, NULL, (void *(*)(void *)) reader, rid + i);

    for(int i = 0;i < 8;i ++)
        pthread_join(rids[i],NULL) ;

    for(int i = 0;i < 2;i ++)
        pthread_join(wid[i],NULL);

    destroy_rw_lock(rwLock);

    return 0;
}
