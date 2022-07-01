#ifndef __HAHA_LOCK_H__
#define __HAHA_LOCK_H__

#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <atomic>
#include "base/noncopyable.h"

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

/* 实现普通锁（不区分读写）的RAII机制 */
template<class T>
struct LockGuard : public noncopyable{
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

template<typename M>
class LockInterface : public noncopyable{
public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
    M& getMutex() { return m_mutex; }
protected:
    M m_mutex;
};


/* 互斥锁 */
class MutexLock : public LockInterface<pthread_mutex_t>{
public:
    typedef LockGuard<MutexLock> RAIILock;
    MutexLock(){
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexLock(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() override{
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() override{
        pthread_mutex_unlock(&m_mutex);
    }
};

/* 自旋锁 */
class SpinLock : public LockInterface<pthread_spinlock_t>{
public:
    typedef LockGuard<SpinLock> RAIILock;
    SpinLock(){
        pthread_spin_init(&m_mutex, 0);
    }

    ~SpinLock(){
        pthread_spin_destroy(&m_mutex);
    }

    void lock() override{
        pthread_spin_lock(&m_mutex);
    }

    void unlock() override{
        pthread_spin_unlock(&m_mutex);
    }
};

/* CAS原子锁 */
class CASLock : public LockInterface<std::atomic_flag>{
public:
    typedef LockGuard<CASLock> RAIILock;
    CASLock(){
        m_mutex.clear();
    }

    ~CASLock(){}

    void lock() override{
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock() override{
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
};


}

#endif