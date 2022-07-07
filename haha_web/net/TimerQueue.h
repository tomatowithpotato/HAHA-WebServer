#ifndef __HAHA_TIMERQUEUE_H__
#define __HAHA_TIMERQUEUE_H__

#include <unistd.h>
#include <memory>
#include <sys/timerfd.h>
#include "log/Log.h"
#include "TimerHeap.h"

namespace haha{

class TimerQueue{
public:
    TimerQueue(int timeFd);
    void push(const Timer &timer);
    void adjust(const Timer &timer);
    void remove(const Timer &timer);
    int getTimeFd() const { return timeFd_; }

    void runOnce(const TimeStamp &t);
    void runForever(const TimeStamp &t);
    void handleTimeout();
private:
    int timeFd_;
    std::unique_ptr<TimerHeap> timerHeap_;
};

}

#endif