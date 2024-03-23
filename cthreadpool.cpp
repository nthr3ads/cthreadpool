#include "cthreadpool.h"
#include <pthread.h>
#include <cerrno>
#include <stdio.h>
#include <time.h>

void *thread_routine(void *arg);

void cthreadpool_init(cthreadpool_t *tp, int thread_nums)
{
    
}
void cthreadpool_add_task(cthreadpool_t *tp, void *(*run)(void *args), void *arg);
void cthreadpool_destroy(cthreadpool_t *tp);