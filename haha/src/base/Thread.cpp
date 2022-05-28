#include "base/Thread.h"
#include <iostream>

namespace haha{

std::atomic<int> Thread::numThread_{0};

static thread_local Thread* cur_thread = nullptr;
static thread_local std::string cur_thread_name = "UNKNOW";

const Thread* Thread::CurrentThread(){
    return cur_thread;
}

const std::string& Thread::CurrentThreadName(){
    return cur_thread_name;
}

Thread::Thread(Task task, const std::string& name)
    :thread_(0)
    ,task_(std::move(task))
    ,name_(name)
    ,started_(false)
    ,joined_(false){
    ++numThread_;
    setDefaultName();
    int ret = pthread_create(&thread_, nullptr, &Thread::run, this);
    if(ret){
        throw std::logic_error("pthread_create error");
    }
    while(!started_); // 等待线程成功运行
}

Thread::~Thread(){
    if(thread_){
        pthread_detach(thread_);
    }
    --numThread_;
}

void Thread::setDefaultName()
{
    if (name_.empty())
    {   
        int curThreadCnt = numThread_;
        // std::cout << curThreadCnt << std::endl;
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread_%d", curThreadCnt);
        name_ = buf;
    }
}

void* Thread::run(void* args){
    Thread* t = static_cast<Thread*>(args);
    cur_thread = t;
    cur_thread_name = t->name_;
    t->started_ = true;
    t->task_();
    return nullptr;
}

void Thread::join(){
    if(thread_){
        int rt = pthread_join(thread_, nullptr);
        if(rt){
            throw std::logic_error("pthread_join error");
        }
        thread_ = 0;
        joined_ = true;
    }
}

}