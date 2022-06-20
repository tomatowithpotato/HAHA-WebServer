#ifndef __HAHA_EVENTLOOP_H__
#define __HAHA_EVENTLOOP_H__

#include "Channel.h"
#include "Epoller.h"
#include "TimerQueue.h"
#include <functional>
#include <memory>

namespace haha{

class EventLoop{

public:
    typedef std::shared_ptr<EventLoop> ptr;
    EventLoop();
    void loop(int idle_timeout_ = -1);

    void addChannel(Channel* channel);
    void modChannel(Channel* channel);
    void delChannel(Channel* channel);

    void addTimer(const Timer& timer);
    void adjustTimer(const Timer& timer);
    void delTimer(const Timer& timer);

    void wakeup();

private:
    void handleWakeup();

private:
    int timeFd_;
    int wakeupFd_;
    Epoller epoller_;
    std::unique_ptr<Channel> timeoutChannel_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::unique_ptr<TimerQueue> timerQueue_;
    // std::thread::id threadId;
};

}


#endif