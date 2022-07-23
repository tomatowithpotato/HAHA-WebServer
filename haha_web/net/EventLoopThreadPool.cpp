#include "EventLoopThreadPool.h"

namespace haha{

EventLoopThreadPool::EventLoopThreadPool(size_t ThreadNum)
    :started_(false)
    ,num_thread_(ThreadNum)
    ,baseLoop_(std::make_shared<EventLoop>())
    ,next_(0)
{

}

void EventLoopThreadPool::start(){
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for(size_t i = 0; i < num_thread_; ++i){
        auto t = std::make_unique<EventLoopThread>();
        HAHA_LOG_INFO(HAHA_LOG_ASYNC_FILE_ROOT()) << "create thread: " << t->getName();
        loops_.push_back(t->getLoop());
        threads_.push_back(std::move(t));
    }
}

EventLoop::ptr EventLoopThreadPool::getNextLoop(){
    baseLoop_->assertInLoopThread();
    EventLoop::ptr loop = baseLoop_;

    if(!loops_.empty()){
        loop = loops_[next_];
        ++next_;
        if (static_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}

}