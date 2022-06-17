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

private:
    int timeFd_;
    Epoller epoller_;
    std::unique_ptr<Channel> timeoutChannel_;
    std::unique_ptr<TimerQueue> timerQueue_;
    // std::thread::id threadId;
};

}


#endif