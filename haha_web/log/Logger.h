#ifndef __HAHA_LOG_LOGGER_H__
#define __HAHA_LOG_LOGGER_H__

#include <memory>
#include <list>
#include <iostream>
#include "base/Mutex.h"
#include "base/Thread.h"
#include "config/config.h"
#include "log/LogInfo.h"
#include "log/LogAppender.h"

// %m -- 消息体
// %p -- level
// %r -- 启动后的时间
// %c -- 日志名称
// %t -- 线程id
// %n -- 回车换行
// %N -- 线程名称
// %d -- 时间
// %f -- 文件名
// %l -- 行号
// %T -- tab

namespace haha{

namespace log{

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef MutexLock MutexType;

    Logger(const std::string &name = "root", LogAppender::ptr appender = nullptr):name_(name),level_(LogLevel::DEBUG){
        formatter_.reset(new LogFormatter(
            haha::config::GET_CONFIG<std::string>("log.default_format", "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%T[%p]%T[%c]%T%f:%l%T%m%n")));
        
        if(appender){
            if(!appender->getFormatter()){
                appender->setFormatter(formatter_);
            }
            appenders_.push_back(appender);
        }
    }

    virtual ~Logger() {}

    virtual void log(LogInfo::ptr info) = 0;

    virtual void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();
    LogLevel::Level getLevel() const {return level_;}
    void setLevel(LogLevel::Level val){level_ = val;}
    const std::string& getName() const {return name_;}

    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string &val);
    LogFormatter::ptr getFormatter();

protected:
    std::string name_;                         // 日志名称
    LogLevel::Level level_;                    // 日志级别
    MutexType mutex_;
    std::list<LogAppender::ptr> appenders_;    // Appender集合
    LogFormatter::ptr formatter_;
};

// 同步日志器
class SyncLogger : public Logger{
public:
    SyncLogger(const std::string &name, SyncLogAppender::ptr appender = nullptr);
    void log(LogInfo::ptr info) override;
protected:
    LogAppender::ptr defaultAppender_;
};

// 异步日志器
class AsyncLogger : public Logger{
public:
    typedef ConditionVariable<MutexType>::ptr Condition;

    AsyncLogger(const std::string &name, int flushInterval, AsyncLogAppender::ptr appender = nullptr);
    ~AsyncLogger(){
        running_ = false;
        cond_->notify_all();
        thread_->join();
    }

    void log(LogInfo::ptr info) override;
    void addAppender(LogAppender::ptr appender) override;

private:
    void task();

private:
    const int flushInterval_;
    MutexType mutex_;
    Condition cond_;
    std::atomic<bool> running_;
    std::unique_ptr<Thread> thread_;
};

}

}

#endif