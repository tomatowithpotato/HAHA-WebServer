#include "log/Logger.h"
#include <iostream>

namespace haha{

namespace log{

void Logger::setFormatter(LogFormatter::ptr val){
    MutexType::RAIILock lock(mutex_);
    formatter_ = val;

    for(auto &i : appenders_){
        // appender还没设置格式，因此把logger的默认格式赋给它
        if(!i->hasFormatter()){
            i->setFormatter(formatter_);
        }
    }
}

void Logger::setFormatter(const std::string &val){
    LogFormatter::ptr new_val(new LogFormatter(val));
    if(new_val->isError()){
        std::cout << "Logger setFormatter name=" << name_
                  << " value=" << val << " invalid formatter"
                  << std::endl;
        return;
    }
    // formatter_ = new_val;
    setFormatter(new_val);
}

LogFormatter::ptr Logger::getFormatter(){
    MutexType::RAIILock lock(mutex_);
    return formatter_;
}

void Logger::addAppender(LogAppender::ptr appender){
    MutexType::RAIILock lock(mutex_);
    if(!appender->getFormatter()){
        appender->setFormatter(formatter_);
    }
    appenders_.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    MutexType::RAIILock lock(mutex_);
    for(auto it = appenders_.begin(); it != appenders_.end(); ++it){
        if(*it == appender){
            appenders_.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders(){
    MutexType::RAIILock lock(mutex_);
    appenders_.clear();
}


SyncLogger::SyncLogger(const std::string &name, SyncLogAppender::ptr appender)
    :Logger(name, appender)
{
    defaultAppender_ = std::make_shared<StdoutSyncLogAppender>();
    defaultAppender_->setFormatter(formatter_);
}


// void SyncLogger::log(LogInfo::ptr info){
//     auto level = info->getLevel();
//     if(level >= level_){
//         if(!appenders_.empty()){
//             for(auto &i : appenders_){
//                 i->append(info);
//             }
//         }
//         else if(defaultAppender_){
//             defaultAppender_->append(info);
//         }
//     }
// }

void SyncLogger::log(LogInfo::ptr info){
    auto level = info->getLevel();
    if(level >= level_){
        if(!appenders_.empty()){
            for(auto &i : appenders_){
                i->append(*info);
            }
        }
        else if(defaultAppender_){
            defaultAppender_->append(*info);
        }
    }
}


AsyncLogger::AsyncLogger(const std::string &name, int flushInterval, AsyncLogAppender::ptr appender)
    :Logger(name, appender),
    flushInterval_(flushInterval),
    cond_(std::make_shared<ConditionVariable<MutexType>>()),
    running_(true),
    thread_(std::make_unique<Thread>([this](){task();}))
{

}

void AsyncLogger::log(LogInfo::ptr info){
    auto level = info->getLevel();
    if(level >= level_){
        for(auto &i : appenders_){
            i->append(*info);
        }
    }
}

void AsyncLogger::addAppender(LogAppender::ptr appender){
    appender->setNotifyFunc([this](){cond_->notify_one();});
    Logger::addAppender(appender);
}

void AsyncLogger::task(){
    while (running_ == true)
    {
        MutexType::RAIILock lock(mutex_);
        cond_->waitForSeconds(mutex_, flushInterval_);
        for(auto &app : appenders_){
            if(!app->empty()){
                app->flush();
            }
        }
    }
    
}

}

}