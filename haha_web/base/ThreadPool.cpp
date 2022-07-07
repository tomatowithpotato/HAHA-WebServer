#include "base/ThreadPool.h"


namespace haha{

ThreadPool::~ThreadPool(){
    isRunning_ = false;
    cond_.notify_all();
    for(auto &t : threads_){
        if(t->joinable()){
            t->join();
        }
    }
}

void ThreadPool::addTask(const Task &task){
    if(!isRunning_)return;

    {
        LockGuard<MutexLock> lock(mtx_);
        taskPool_.emplace(task);
    }
    cond_.notify_one();
}

void ThreadPool::start(){
    isRunning_ = true;
    for(size_t i = 0; i < num_thread_; ++i){
        Thread::ptr p = std::make_shared<Thread>(std::bind(&ThreadPool::run, this));
        threads_.emplace_back(p);
    }
}

void ThreadPool::run(){
    Task task;
    while(isRunning_){
        {
            // LockGuard<MutexLock> lock_gaurd(mtx_);
            MutexLock::RAIILock lock_gaurd(mtx_);
            while(isRunning_ && taskPool_.empty()){
                cond_.wait(mtx_);
            }
            if(!isRunning_)break;
            task = taskPool_.front();
            taskPool_.pop();
        }
        task();
    }
}

}