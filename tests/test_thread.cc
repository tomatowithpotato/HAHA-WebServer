#include "../haha/include/base/Thread.h"
#include <iostream>
#include <vector>
#include <memory>

void doWork(){
    std::cout << haha::Thread::CurrentThreadName() << std::endl;
}

int main(){
    std::vector<std::shared_ptr<haha::Thread>> threads;
    for(int i = 0; i < 10; ++i){
        std::shared_ptr<haha::Thread> t = std::make_shared<haha::Thread>(doWork);
        threads.push_back(t);
    }

    std::cout << haha::Thread::getThreadCount() << std::endl;

    for(int i = 0; i < threads.size(); ++i){
        threads[i]->join();
    }
    return 0;
}