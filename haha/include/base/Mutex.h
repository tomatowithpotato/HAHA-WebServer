#ifndef __HAHA_LOCK_H__
#define __HAHA_LOCK_H__

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic>

namespace haha{

/* 信号量 */
class Semaphore{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    sem_t m_semaphore;
};

/* 实现锁的RALL机制 */
template<class T>
struct LockGuard{
public:
    LockGuard(T& mutex)
        :m_mutex(mutex){
        m_mutex.lock();
        m_locked = true;
    }

    ~LockGuard(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

    T& getLock(){
        return m_mutex;
    }

private:
    T& m_mutex;
    bool m_locked;
};

/* 互斥锁 */
class MutexLock{
friend class ConditionVariable;
public:
    typedef LockGuard<MutexLock> RallLock;
    MutexLock(){
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexLock(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock(){
        pthread_mutex_lock(&m_mutex);
    }

    void unlock(){
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

/* 自旋锁 */
class SpinLock{
public:
    typedef LockGuard<SpinLock> RallLock;
    SpinLock(){
        pthread_spin_init(&m_mutex, 0);
    }

    ~SpinLock(){
        pthread_spin_destroy(&m_mutex);
    }

    void lock(){
        pthread_spin_lock(&m_mutex);
    }

    void unlock(){
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

/* CAS原子锁 */
class CASLock{
public:
    typedef LockGuard<CASLock> RallLock;
    CASLock(){
        m_mutex.clear();
    }

    ~CASLock(){}

    void lock(){
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock(){
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
private:
    volatile std::atomic_flag m_mutex;
};


}

#endif