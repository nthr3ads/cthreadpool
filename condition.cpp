#include "condition.h"

int cond_init(condition_t *cd)
{
    int status;
    status = pthread_mutex_init(&cd->mtx, NULL);
    if (status != 0)
    {
        printf("Error: pthread_mutex_init failed, return val: %d\n", status);
        return status;
    }
}

int cond_lock(condition_t *cd)
{
    return pthread_mutex_lock(&cd->mtx);
}

int cond_unlock(condition_t *cd)
{
    return pthread_mutex_unlock(&cd->mtx);    
}

int cond_wait(condition_t *cd)
{
    return pthread_cond_wait(&cd->cond, &cd->mtx);
}

int cond_record_wait_time(condition_t *cd, const timespec *abstime)
{

}

int cond_signal(condition_t *cd)
{

}

int cond_broadcast(condition_t *cd)
{

}

int cond_destroy(condition_t *cd)
{
    
}