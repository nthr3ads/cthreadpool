#ifndef C_THREADPOOL_CONDITION_H

#include <pthread.h>
#include <stdio.h>

typedef struct condition
{
    pthread_mutex_t mtx;
    pthread_cond_t cond;    
} condition_t;

int cond_init(condition_t *cd);
int cond_lock(condition_t *cd);
int cond_unlock(condition_t *cd);
int cond_wait(condition_t *cd);
int cond_record_wait_time(condition_t *cd, const timespec *abstime);
int cond_signal(condition_t *cd);
int cond_broadcast(condition_t *cd);
int cond_destroy(condition_t *cd);

#endif // !C_THREADPOOL_CONDITION_H