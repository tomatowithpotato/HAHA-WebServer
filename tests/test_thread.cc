#include "../haha/include/base/Thread.h"
#include "../haha/include/base/Mutex.h"
#include <unistd.h>
#include <iostream>
#include <vector>
#include <memory>

int count = 0;
haha::CASLock mutex_;

void doWork(){
    haha::CASLock::RAIILock lock(mutex_);
    usleep(5000);
    ++count;
    std::cout << haha::Thread::getCurrentThreadName() << std::endl;
}

int main(){
    std::vector<std::shared_ptr<haha::Thread>> threads;
    for(int i = 0; i < 100; ++i){
        std::shared_ptr<haha::Thread> t = std::make_shared<haha::Thread>(doWork);
        threads.push_back(t);
    }

    std::cout << haha::Thread::getThreadCount() << std::endl;

    for(int i = 0; i < threads.size(); ++i){
        threads[i]->join();
    }

    std::cout << count << std::endl;

    return 0;
}