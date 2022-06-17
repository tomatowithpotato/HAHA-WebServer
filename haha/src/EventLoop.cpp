#include "EventLoop.h"

namespace haha{

EventLoop::EventLoop()
    :timeFd_(::timerfd_create(CLOCK_REALTIME, 0))
    ,timeoutChannel_(std::make_unique<Channel>(this, timeFd_))
    ,timerQueue_(std::make_unique<TimerQueue>(timeFd_)){
    timeoutChannel_->setEvents(EPOLLIN);
    timeoutChannel_->setReadCallback(std::bind(&TimerQueue::handleTimeout, timerQueue_.get()));
}

void EventLoop::loop(int idle_timeout_){
    if(idle_timeout_ > 0){
        /* 如果时间间隔小于等于0，那就不做定时检查了 */
        addChannel(timeoutChannel_.get());
        timerQueue_->runForever(TimeStamp::second(idle_timeout_));
    }

    while(true){
        int num_events = epoller_.wait();
        for(int i = 0; i < num_events; ++i){
            Channel* activeChannel = epoller_.getActiveChannel(i);
            uint32_t revent = epoller_.getReturnEvents(i);
            activeChannel->handleEvents(revent);
        }
    }
}


void EventLoop::addChannel(Channel* channel){
    epoller_.addChannel(channel);
    
}

void EventLoop::modChannel(Channel* channel){
    epoller_.modChannel(channel);
}

void EventLoop::delChannel(Channel* channel){
    epoller_.delChannel(channel);
}

void EventLoop::addTimer(const Timer& timer){
    timerQueue_->push(timer);
}

void EventLoop::adjustTimer(const Timer& timer){
    timerQueue_->adjust(timer);
}

void EventLoop::delTimer(const Timer& timer){
    timerQueue_->remove(timer);
}

}