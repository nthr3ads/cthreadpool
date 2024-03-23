#include "cthreadpool.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_THREADPOOL 500

void *my_task_func_call(void *arg)
{
    printf("Info: thread %ld is working on task %d\n", (u_int64_t)pthread_self(), *reinterpret_cast<int *>(arg));
    sleep(5);
    free(arg);
    return NULL;
}

int main(int argc, char *argv[])
{
    cthreadpool_t tp;
    cthreadpool_init(&tp, MAX_THREADPOOL);
    for (int i = 0; i < 1000; i++)
    {
        int *arg = reinterpret_cast<int *>(malloc(sizeof(int)));
        *arg = i;
        cthreadpool_add_task(&tp, my_task_func_call, arg);
    }
    cthreadpool_destroy(&tp);
    return 0;
}