/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */ 
#ifndef __HAHA_TIMERHEAP_H__
#define __HAHA_TIMERHEAP_H__

#include <queue>
#include <unordered_map>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include "base/TimeStamp.h"
#include "base/Mutex.h"

namespace haha{

typedef std::function<void()> TimeoutCallBack;

struct Timer {
    int fd;
    TimeStamp expire;
    TimeoutCallBack cb;
    Timer(int fd, const TimeStamp &expire, const TimeoutCallBack cb){
        this->fd = fd;
        this->expire = expire;
        this->cb = cb;
    }
    bool operator<(const Timer& t) {
        return expire < t.expire;
    }
};


class TimerHeap {
public:
    TimerHeap() { heap_.reserve(64); }

    ~TimerHeap() { clear(); }

    void adjust(const Timer &timer);

    void push(const Timer &timer);

    void clear();

    void pop();

    Timer& top() { return heap_.front(); }

    bool empty() const { return heap_.empty(); }

    size_t size() const { return heap_.size(); }

private:
    void del_(size_t i);
    
    void siftup_(size_t i);

    bool siftdown_(size_t index, size_t n);

    void SwapNode_(size_t i, size_t j);

    std::vector<Timer> heap_;

    // 记录定时器在堆中的位置
    std::unordered_map<int, size_t> ref_;

    MutexLock mtx_;
};

}

#endif //HEAP_TIMER_H