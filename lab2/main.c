#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "contiguous.h"

#define SYSTEM_ALLOCATION 0
#define MY_ALLOCATION 1

static mem_pool *pool = NULL;

FILE *output = NULL;
int ignore = 0 ;
FILE *check_output = NULL;

clock_t alloc_clocks = 0;
clock_t free_clocks = 0;
int alloc_times = 0;
int free_times = 0;
int max_fragments = 0;

void *my_malloc(size_t s){
    return p_allocate(pool,s);
}

void my_free(void *data){
    p_free(pool,data);
}

void *(*malloc_ptr)(size_t);
void (*free_ptr)(void *);

// 封装分配函数
// 加上计时规则
void *Malloc(size_t s){
    void *data;
    clock_t start,end;
    start = clock();
    data = malloc_ptr(s);
    end = clock();
    alloc_times ++;
    alloc_clocks += (end - start);
    return data;
}

// 封装释放函数
// 加上计时规则
void Free(void *data){
    clock_t start,end;
    start = clock();
    free_ptr(data);
    end = clock();
    free_times ++;
    free_clocks += (end - start);
}

void summary(FILE *fp){
    fprintf(fp,"allocate times:%d\n",alloc_times);
    fprintf(fp,"free times:%d\n",free_times);
    fprintf(fp,"total allocate time: %lf s\n",(double)(alloc_clocks) / CLOCKS_PER_SEC);
    fprintf(fp,"total free time: %lf s\n",(double)(free_clocks) / CLOCKS_PER_SEC);
    fprintf(fp,"max fragments:%d\n",max_fragments);
}

void init(int type,int strategy){
    if(type == MY_ALLOCATION) {
        pool = create_mem_pool(POOL_SIZE, strategy);
        malloc_ptr = my_malloc;
        free_ptr = my_free;
    } else{  // SYSTEM_ALLOCATION
        malloc_ptr = malloc;
        free_ptr = free;
    }
}

static void arg_init(int argc,char **argv){
    char *optstr = "It:s:o:c:";   // -t type -s strategy -o output_file_for_free_list
    int ch = 0;
    int type = SYSTEM_ALLOCATION,strategy = FIRST_FIT;
    while((ch = getopt(argc,argv,optstr)) != -1){
        switch (ch) {
            case 'o':
                if(strcmp(optarg,"stdout") == 0)
                    output = stdout;
                else
                    output = fopen(optarg,"w");
                break;
            case 'c':
                if(strcmp(optarg,"stdout") == 0)
                    check_output = stdout;
                else
                    check_output = fopen(optarg,"w");
                break;
            case 't':
                if(strcmp(optarg,"mine") == 0)
                    type = MY_ALLOCATION;
                break;
            case 's':
                if(strcmp(optarg,"first") == 0)
                    strategy = FIRST_FIT;
                else if(strcmp(optarg,"best") == 0)
                    strategy = BEST_FIT;
                else
                    strategy = WORST_FIT;
                break;
            case 'I':
                ignore = 1;
                break;
            default:
                break;
        }
    }

    init(type,strategy);

}

int main(int argc,char **argv) {
    arg_init(argc,argv);

    int *array[10000];
    int pos,num,n,size;
    char cmd[50];
    for(;;){
        if(!ignore) printf(">>");
        scanf("%s",cmd);
        if(strcmp(cmd,"alloc") == 0){
            scanf("%d%d%d",&pos,&num,&size);
            array[pos] = Malloc(size);
            *array[pos] = num;
        }else if(strcmp(cmd,"check") == 0){
            scanf("%d",&pos);
            fprintf(check_output,"%d\n",*array[pos]);
        }else if(strcmp(cmd,"free") == 0){
            scanf("%d",&pos);
            Free(array[pos]);
        } else
            break;

        if(pool != NULL && !ignore) {
            print_free_block(output, pool);
        }

        if(pool != NULL) {
            int tmp = get_fragments(pool);
            // fprintf(output,"%d\n",tmp);
            if (tmp > max_fragments)
                max_fragments = tmp;
        }
    }

    summary(output);

    if(pool != NULL)
        destroy_pool(pool);

    if(output != stdout)
        fclose(output);

    if(check_output != stdout)
        fclose(check_output);

    return 0;
}
