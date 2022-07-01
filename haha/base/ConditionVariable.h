#ifndef __HAHA_CONDITIONVARIABLE_H__
#define __HAHA_CONDITIONVARIABLE_H__

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic>
#include <memory>
#include "Mutex.h"
#include "noncopyable.h"

namespace haha
{

template<typename MutexType>
class ConditionVariable : public noncopyable{
public:
    typedef std::shared_ptr<ConditionVariable<MutexType>> ptr;

    ConditionVariable(){
        pthread_cond_init(&cond_, nullptr);
    }

    ~ConditionVariable(){
        pthread_cond_destroy(&cond_);
    }

    void wait(MutexType &mutex){
        pthread_cond_wait(&cond_, &(mutex.getMutex()));
    }

    bool waitForSeconds(MutexType &mutex, double seconds){
        struct timespec abstime;
        // FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
        clock_gettime(CLOCK_REALTIME, &abstime);

        const int64_t kNanoSecondsPerSecond = 1000000000;
        int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

        abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
        abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

        return ETIMEDOUT == pthread_cond_timedwait(&cond_, &(mutex.getMutex()), &abstime);
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