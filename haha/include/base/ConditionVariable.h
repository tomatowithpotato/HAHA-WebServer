#ifndef __HAHA_CONDITIONVARIABLE_H__
#define __HAHA_CONDITIONVARIABLE_H__

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic>
#include "Mutex.h"

namespace haha
{

class ConditionVariable{
public:
    ConditionVariable(){
        pthread_cond_init(&cond_, nullptr);
    }

    ~ConditionVariable(){
        pthread_cond_destroy(&cond_);
    }

    void wait(MutexLock &mutex){
        pthread_cond_wait(&cond_, &(mutex.m_mutex));
    }

    void notify_one(){
        pthread_cond_signal(&cond_);
    }

    void notify_all(){
        pthread_cond_broadcast(&cond_);
    }
private:
    pthread_cond_t cond_;
};

} // namespace haha

#endif