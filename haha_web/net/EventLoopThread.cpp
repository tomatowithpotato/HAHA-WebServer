#include "EventLoopThread.h"
#include "log/Log.h"

namespace haha{

EventLoopThread::EventLoopThread(const std::string& name)
    :exiting_(false)
    ,loop_ok_(false)
    ,loop_(nullptr)
    ,thread_(std::make_unique<Thread>(std::bind(&EventLoopThread::work, this), name))
{
    while(!loop_ok_);
}

EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    loop_->quit();
    if(thread_->joinable()){
        thread_->join();
    }
    HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "~EventLoopThread";
}

void EventLoopThread::work(){
    HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "start work";
    loop_ = std::make_shared<EventLoop>();
    loop_ok_ = true;
    loop_->loop(5);
}

}