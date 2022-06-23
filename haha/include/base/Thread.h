#ifndef __HAHA_THREAD_H__
#define __HAHA_THREAD_H__

#include "noncopyable.h"
#include <functional>
#include <memory>
#include <thread>
#include <string>
#include <atomic>
#include <pthread.h>

namespace haha{

class Thread : noncopyable{
public:
    typedef std::shared_ptr<Thread> ptr;
    typedef std::function<void ()> Task;

    explicit Thread(Task task, const std::string& name = "");
    ~Thread();

    void start();
    void join(); // return pthread_join()
    bool joinable() { return !joined_ && thread_; }
    const std::string& name() const { return name_; }

    static const Thread* CurrentThread();
    static const std::string& CurrentThreadName();

    bool isStarted() const { return started_; }
    static const int getThreadCount() { return numThread_; }

private:
    void setDefaultName();
    static void* run(void*);

private:
    pthread_t                        thread_;     // 线程号
    bool                            joined_;     // 是否要join
    Task                            task_;      // 任务
    std::string                     name_;      // 名字
    volatile bool                            started_;    // 已启动
    static std::atomic<int>         numThread_;     // 线程数量
};

}

#endif