#include "EventLoop.h"
#include <sys/eventfd.h>

namespace haha{

int createEventfd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        HAHA_LOG_ERROR(HAHA_LOG_ROOT()) << "Failed in create eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    :timeFd_(::timerfd_create(CLOCK_REALTIME, 0))
    ,wakeupFd_(createEventfd())
    ,quit_(false)
    ,timeoutChannel_(std::make_unique<Channel>(this, timeFd_))
    ,wakeupChannel_(std::make_unique<Channel>(this, wakeupFd_))
    ,timerQueue_(std::make_unique<TimerQueue>(timeFd_))
    ,threadId_(Thread::getCurrentThreadId())
    ,callingPendingTasks_(false)
{    
    timeoutChannel_->setEvents(EPOLLIN | EPOLLET);
    timeoutChannel_->setReadCallback(std::bind(&TimerQueue::handleTimeout, timerQueue_.get()));
    wakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
}

void EventLoop::loop(int idle_timeout_){
    assertInLoopThread();
    
    if(idle_timeout_ > 0){
        /* 如果时间间隔小于等于0，那就不做定时检查了 */
        addChannel(timeoutChannel_.get());
        timerQueue_->runForever(TimeStamp::second(idle_timeout_));
    }
    addChannel(wakeupChannel_.get());

    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "start loop";

    while(!quit_){
        int num_events = epoller_.wait();
        for(int i = 0; i < num_events; ++i){
            Channel* activeChannel = epoller_.getActiveChannel(i);
            uint32_t revent = epoller_.getReturnEvents(i);
            activeChannel->handleEvents(revent);
        }
        doPendingTasks();
    }
}


void EventLoop::addChannel(Channel* channel){
    runInLoop([this, channel](){
        epoller_.addChannel(channel);
    });
}

void EventLoop::modChannel(Channel* channel){
    runInLoop([this, channel](){
        epoller_.modChannel(channel);
    });
}

void EventLoop::delChannel(Channel* channel){
    runInLoop([this, channel](){
        epoller_.delChannel(channel);
    });
}

void EventLoop::addTimer(const Timer& timer){
    runInLoop([this, &timer](){
        timerQueue_->push(timer);
    });
}

void EventLoop::adjustTimer(const Timer& timer){
    runInLoop([this, &timer](){
        timerQueue_->adjust(timer);
    });
}

void EventLoop::delTimer(const Timer& timer){
    runInLoop([this, &timer](){
        timerQueue_->remove(timer);
    });
}

void EventLoop::quit(){
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, (void*)(&one), sizeof(one));
    if (n != sizeof(one)) {
        HAHA_LOG_ERROR(HAHA_LOG_ROOT()) << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleWakeup(){
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, (void*)(&one), sizeof(one));
    if (n != sizeof(one)) {
        HAHA_LOG_ERROR(HAHA_LOG_ROOT()) << "EventLoop::wakeup() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::doPendingTasks(){
    std::vector<Task> tasks;
    callingPendingTasks_ = true;
    {
        // MutexLock::RAIILock lock(mutex_);
        SpinLock::RAIILock lock(mutex_);
        tasks.swap(pendingTasks_);
    }
    for(const auto &task : tasks){
        task();
    }
    callingPendingTasks_ = false;
}

void EventLoop::queueInLoop(Task task){
    {
        // MutexLock::RAIILock lock(mutex_);
        SpinLock::RAIILock lock(mutex_);
        pendingTasks_.push_back(std::move(task));
    }

    if (!isInLoopThread() || callingPendingTasks_)
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Task task){
    if(isInLoopThread()){
        task();
    }
    else{
        queueInLoop(std::move(task));
    }
}

void EventLoop::assertInLoopThread(){
    if(!isInLoopThread()){
        HAHA_LOG_ERROR(HAHA_LOG_ROOT()) << "EventLoop was created in threadId_ = "
            << threadId_ << ", but current thread id = " << Thread::getCurrentThreadId();
        assert(isInLoopThread());
    }
}

}