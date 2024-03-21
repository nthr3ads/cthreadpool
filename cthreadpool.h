#ifndef C_THREADPOOL_H_
#define C_THREADPOOL_H_

#include "condition.h"

typedef struct task
{
    void *(*run)(void *args);
    void *arg;
    struct task *next;
} task_t;

typedef struct cthreadpool
{
    condition_t ready;
    task_t *head, *tail;
    int counter, idle, max_threads, quit;
} cthreadpool_t;

void cthreadpool_init(cthreadpool_t *tp, int thread_nums);
void cthreadpool_add_task(cthreadpool_t *tp, void *(*run)(void *args), void *arg);
void cthreadpool_destroy(cthreadpool_t *tp);

#endif // C_THREADPOOL_H_