#ifndef __HAHA_EVENTLOOPTHREADPOOL_H__
#define __HAHA_EVENTLOOPTHREADPOOL_H__

#include <vector>
#include <string>
#include "base/noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

namespace haha{

class EventLoopThreadPool : noncopyable{
public:
    static EventLoopThreadPool& getInstance(){
        static EventLoopThreadPool pool;
        return pool;
    }
    void start();
    bool isStarted() const { return started_; }
    EventLoop::ptr getBaseLoop() const { return baseLoop_; }
    EventLoop::ptr getNextLoop();

private:
    EventLoopThreadPool(size_t ThreadNum = 4);

private:
    bool started_;
    size_t num_thread_;
    EventLoop::ptr baseLoop_;
    std::vector<EventLoop::ptr> loops_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    int next_;
};

}

#endif