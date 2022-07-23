#include "../haha/include/base/Log.h"
#include <iostream>
#include <vector>
#include <memory>

void log_info(haha::Logger::ptr logger){
    auto level = haha::LogLevel::Level::INFO;
    if(logger->getLevel() <= level){
        haha::LogEventWrap(logger, haha::LogEvent::ptr(new haha::LogEvent(logger, level, 
                        __FILE__, __LINE__, 0, haha::GetThreadId(), 
                        time(0), haha::Thread::CurrentThreadName()))).getSS();
    }
}

int main(){
    // HAHA_LOG_INFO(HAHA_LOG_ASYNC_FILE_ROOT()) << "hello";
    log_info(HAHA_LOG_ASYNC_FILE_ROOT());
    return 0;
}