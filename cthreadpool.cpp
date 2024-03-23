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
    int n_sta = cond_init(&tp->ready);
    if (n_sta == 0)
        printf("Info : init the thread pool success!\n");
    else
        printf("Error: init the thread pool failed, status: %d\n", n_sta);
    tp->head = NULL;
    tp->tail = NULL;
    tp->counter = 0;
    tp->idle = 0;
    tp->max_threads = thread_nums;
    tp->quit = 0;
}

void cthreadpool_add_task(cthreadpool_t *tp, void *(*run)(void *args), void *arg)
{
    task_t *new_task = reinterpret_cast<task_t *>(malloc(sizeof(task_t)));
    new_task->run = run;
    new_task->arg = arg;
    new_task->next = NULL;
    cond_lock(&tp->ready);
    if (tp->head == NULL)
        tp->head = new_task;
    else
        tp->tail->next = new_task;
    tp->tail = new_task;
    if (tp->idle > 0)
        cond_signal(&tp->ready);
    else if (tp->counter < tp->max_threads)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine, tp);
        tp->counter++;
    }
    else
        printf("Warn: no idle thread, please wait ...\n");
    cond_unlock(&tp->ready);
}


void cthreadpool_destroy(cthreadpool_t *tp)
{
    if (tp->quit)
        return;
    cond_lock(&tp->ready);
    tp->quit = 1;
    if (tp->counter > 0)
    {
        if (tp->idle > 0)
            cond_broadcast(&tp->ready);
    }
    while (tp->counter > 0)
        cond_wait(&tp->ready);
    cond_unlock(&tp->ready);
    cond_destroy(&tp->ready);
}

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