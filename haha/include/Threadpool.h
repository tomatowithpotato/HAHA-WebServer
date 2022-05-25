#ifndef __HAHA_THREADPOOL_H__
#define __HAHA_THREADPOOL_H__

#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "base/noncopyable.h"

namespace haha{

using Task = std::function<void()>;

class ThreadPool : noncopyable{
public:
    static ThreadPool& getInstance(){
        static ThreadPool pool;
        return pool;
    }
    ~ThreadPool();
    void addTask(const Task &task);
    void start();
    bool isRunning() const {return isRunning_;}
    size_t getThreadNum() const {return num_thread_;}

private:
    ThreadPool(size_t ThreadNum = 8): num_thread_(ThreadNum){}
    void run();

private:
    std::queue<Task> taskPool_;
    std::vector<std::thread> threads_;
    size_t num_thread_;
    std::atomic<bool> isRunning_;
    std::mutex mtx_;
    std::condition_variable cond_;
};

}

#endif