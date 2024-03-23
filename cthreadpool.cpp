#include "cthreadpool.h"
#include <pthread.h>
#include <cerrno>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <cstdint>

void *thread_routine(void *arg);

void cthreadpool_init(cthreadpool_t *tp, int thread_nums)
{
    
}
void cthreadpool_add_task(cthreadpool_t *tp, void *(*run)(void *args), void *arg);
void cthreadpool_destroy(cthreadpool_t *tp);

void *thread_routine(void *arg)
{
    struct timespec abs;
    bool timeout;
    printf("Info: create thread, and the thread id is: %ld\n", (u_int64_t) pthread_self());
    cthreadpool_t *pool = reinterpret_cast<cthreadpool_t *>(arg);
    while (true)
    {
        timeout = false;
        cond_lock(&pool->ready);
        pool->idle++;
        while (pool->head == NULL && !pool->quit)
        {
            printf("Info: thread %ld is waiting for a task\n", (u_int64_t) pthread_self());
            clock_gettime(CLOCK_REALTIME, &abs);
            abs.tv_sec += 2;
            int status = cond_record_wait_time(&pool->ready, &abs);
            if (status == ETIMEDOUT)
            {
                printf("Info: thread %ld wait timed out \n", (u_int64_t) pthread_self());
                timeout = true;
                break;
            }
        }
        pool->idle--;
        if (pool->head != NULL)
        {
            task_t *t = pool->head;
            pool->head = t->next;
            cond_unlock(&pool->ready);
            t->run(t->arg);
            free(t);
            cond_lock(&pool->ready);
        }
        if (pool->quit && pool->head == NULL)
        {
            pool->counter--;
            if (pool->counter == 0)
                cond_signal(&pool->ready);
            cond_unlock(&pool->ready);
            break;
        }
        if (timeout)
        {
            pool->counter--;
            cond_unlock(&pool->ready);
            break;
        }
        cond_unlock(&pool->ready);
    }
        printf("Info : thread %ld quit\n", (u_int64_t) pthread_self());
        return NULL;
}