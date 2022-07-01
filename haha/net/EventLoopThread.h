#ifndef __HAHA_EVENTLOOPTHREAD_H__
#define __HAHA_EVENTLOOPTHREAD_H__

#include <atomic>
#include "base/Thread.h"
#include "EventLoop.h"

namespace haha{

class EventLoopThread{
public:
    typedef std::shared_ptr<EventLoopThread> ptr;
    EventLoopThread(const std::string& name = "");
    ~EventLoopThread();
    EventLoop::ptr getLoop() const { return loop_; }
    const std::string& getName() const { return thread_->name(); }

private:
    void work();

private:
    bool exiting_;
    volatile bool loop_ok_;
    EventLoop::ptr loop_;
    std::unique_ptr<Thread> thread_;
};

}

#endif