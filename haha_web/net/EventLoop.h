#ifndef __HAHA_EVENTLOOP_H__
#define __HAHA_EVENTLOOP_H__

#include <functional>
#include <memory>
#include <atomic>
#include "base/Mutex.h"
#include "base/Thread.h"
#include "Channel.h"
#include "Epoller.h"
#include "TimerQueue.h"

namespace haha{

class EventLoop{

public:
    typedef std::shared_ptr<EventLoop> ptr;
    typedef std::weak_ptr<EventLoop> weak_ptr;
    typedef std::function<void()> Task;

    EventLoop();
    void loop(int idle_timeout_ = -1);

    void addChannel(Channel* channel);
    void modChannel(Channel* channel);
    void delChannel(Channel* channel);

    void addTimer(const Timer& timer);
    void adjustTimer(const Timer& timer);
    void delTimer(const Timer& timer);

    void wakeup();
    void quit();

    bool isInLoopThread() const { return threadId_ == Thread::getCurrentThreadId(); }
    void runInLoop(Task task);
    void assertInLoopThread();

private:
    void handleWakeup();
    void doPendingTasks();
    void queueInLoop(Task task);

private:
    int timeFd_;
    int wakeupFd_;
    bool quit_;
    Epoller epoller_;
    std::unique_ptr<Channel> timeoutChannel_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::unique_ptr<TimerQueue> timerQueue_;
    
    // MutexLock mutex_;
    SpinLock mutex_;
    Thread::ID threadId_;
    std::vector<Task> pendingTasks_;
    std::atomic_bool callingPendingTasks_;
};

}


#endif