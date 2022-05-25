#include "Threadpool.h"


namespace haha{

ThreadPool::~ThreadPool(){
    isRunning_ = false;
    cond_.notify_all();
    for(auto &t : threads_){
        if(t.joinable()){
            t.join();
        }
    }
}

void ThreadPool::addTask(const Task &task){
    if(!isRunning_)return;

    {
        std::unique_lock<std::mutex> lock(mtx_);
        taskPool_.emplace(task);
    }
    cond_.notify_one();
}

void ThreadPool::start(){
    isRunning_ = true;
    for(size_t i = 0; i < num_thread_; ++i){
        threads_.emplace_back(std::thread(std::bind(&ThreadPool::run, this)));
    }
}

void ThreadPool::run(){
    Task task;
    while(isRunning_){
        {
            std::unique_lock<std::mutex> lock(mtx_);
            while(isRunning_ && taskPool_.empty()){
                cond_.wait(lock);
            }
            if(!isRunning_)break;
            task = taskPool_.front();
            taskPool_.pop();
        }
        task();
    }
}

}