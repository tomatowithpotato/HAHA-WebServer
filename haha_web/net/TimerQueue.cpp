#include "TimerQueue.h"
#include <iostream>

namespace haha{

TimerQueue::TimerQueue(int timeFd)
    :timeFd_(timeFd)
    ,timerHeap_(std::make_unique<TimerHeap>()){

}

void TimerQueue::push(const Timer& timer) {
    timerHeap_->push(timer);
}

void TimerQueue::adjust(const Timer& timer) {
    timerHeap_->adjust(timer);
}

void TimerQueue::remove(const Timer &timer){
    timerHeap_->remove(timer);
}

// run once
void TimerQueue::runOnce(const TimeStamp &t) {
    uint64_t second = t.second();
    uint64_t nanosecond = (t.microsecond() - t.second() * 1000000) * 100;
    timespec now;
    ::clock_gettime(CLOCK_REALTIME, &now);
    itimerspec itsp;
    itsp.it_value.tv_sec = now.tv_sec + second;
    itsp.it_value.tv_nsec = now.tv_nsec + nanosecond;
    itsp.it_interval.tv_sec = 0;
    itsp.it_interval.tv_nsec = 0;

    ::timerfd_settime(timeFd_, TFD_TIMER_ABSTIME, &itsp, nullptr);
}

// run forever
void TimerQueue::runForever(const TimeStamp &t) {
    uint64_t second = t.second();
    uint64_t nanosecond = (t.microsecond() - t.second() * 1000000) * 100;
    timespec now;
    ::clock_gettime(CLOCK_REALTIME, &now);
    itimerspec itsp;
    itsp.it_value.tv_sec = now.tv_sec + second;
    itsp.it_value.tv_nsec = now.tv_nsec + nanosecond;
    itsp.it_interval.tv_sec = second;
    itsp.it_interval.tv_nsec = nanosecond;

    ::timerfd_settime(timeFd_, TFD_TIMER_ABSTIME, &itsp, nullptr);
}

void TimerQueue::handleTimeout() {
    HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "timeout";
    
    uint64_t one;
    int n = ::read(timeFd_, &one, sizeof(one));
    if(n != sizeof(one)){
        HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "read time fd error?";
    }

    TimeStamp now = TimeStamp::now();
    while (!timerHeap_->empty()) {
        auto timer = timerHeap_->top();
        if (timer.expire <= now) {
            timer.cb();
            timerHeap_->pop();
        } 
        else {
            break;
        }
    }
}

}