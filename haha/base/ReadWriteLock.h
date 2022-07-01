#ifndef __HAHA_READWRITELOCK_H__
#define __HAHA_READWRITELOCK_H__

#include <pthread.h>

namespace haha{

/* *********************************************读写锁********************************************* */

// 读锁的RAII机制
template<class T>
class ReadLockGuard{
public:
    ReadLockGuard(T& mutex)
        :m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }
    ~ReadLockGuard(){
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

// 写锁的RAII机制
template<class T>
class WriteLockGuard{
public:
    WriteLockGuard(T& mutex)
        :m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;
    }
    ~WriteLockGuard(){
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

/* 读写锁 */
class ReadWriteLock{
public:
    typedef ReadLockGuard<ReadWriteLock> RAIIReadLock;
    typedef WriteLockGuard<ReadWriteLock> RAIIWriteLock;
    ReadWriteLock(){
        pthread_rwlock_init(&m_mutex, nullptr);
    }
    void rdlock(){
        pthread_rwlock_rdlock(&m_mutex);
    }
    void wrlock(){
        pthread_rwlock_wrlock(&m_mutex);
    }
    void unlock(){
        pthread_rwlock_unlock(&m_mutex);
    }
private:
    pthread_rwlock_t m_mutex;
};

}

#endif