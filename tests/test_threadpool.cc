#include "base/ThreadPool.h"
#include "log/Log.h"
#include <iostream>
#include <unistd.h>
#include <set>

haha::ThreadPool &pool = haha::ThreadPool::getInstance();

void work(int i){
    // sleep(1);
    usleep(100 * 1000);
    HAHA_LOG_INFO(HAHA_LOG_ROOT());
}

int main(){
    pool.start();
    for(int i = 0; i < 10; ++i){
        for(int j = 0; j < 1000; ++j){
            pool.addTask(std::bind(work, j));
        }
        sleep(1);
    }
    
    return 0;
}