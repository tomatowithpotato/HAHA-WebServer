#ifndef __HAHA_LOG_H__
#define __HAHA_LOG_H__

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <list>
#include <map>
#include <functional>
#include <stdio.h>
#include <stdarg.h>
#include "base/Singleton.h"
#include "base/Mutex.h"
#include "base/util.h"
#include "base/Thread.h"

namespace haha{


#define HAHA_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
    haha::LogEventWrap(logger, haha::LogEvent::ptr(new haha::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, haha::GetThreadId(), \
                        time(0), haha::Thread::getCurrentThreadName()))).getSS()

#define HAHA_LOG_DEBUG(logger)  HAHA_LOG_LEVEL(logger, haha::LogLevel::Level::DEBUG)
#define HAHA_LOG_INFO(logger)  HAHA_LOG_LEVEL(logger, haha::LogLevel::Level::INFO)
#define HAHA_LOG_WARN(logger)  HAHA_LOG_LEVEL(logger, haha::LogLevel::Level::WARN)
#define HAHA_LOG_ERROR(logger)  HAHA_LOG_LEVEL(logger, haha::LogLevel::Level::ERROR)
#define HAHA_LOG_FATAL(logger)  HAHA_LOG_LEVEL(logger, haha::LogLevel::Level::FATAL)


#define HAHA_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
    haha::LogEventWrap(haha::LogEvent::ptr(new haha::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, haha::GetThreadId(), \
                        time(0), haha::Thread::getCurrentThreadName()))).getLogEvent()->format(fmt, __VA_ARGS__)

#define HAHA_LOG_FMT_DEBUG(logger, fmt, ...)  HAHA_LOG_FMT_LEVEL(logger, haha::LogLevel::Level::DEBUG, fmt, __VA_ARGS__)
#define HAHA_LOG_FMT_INFO(logger, fmt, ...)  HAHA_LOG_FMT_LEVEL(logger, haha::LogLevel::Level::INFO, fmt, __VA_ARGS__)
#define HAHA_LOG_FMT_WARN(logger, fmt, ...)  HAHA_LOG_FMT_LEVEL(logger, haha::LogLevel::Level::WARN, fmt, __VA_ARGS__)
#define HAHA_LOG_FMT_ERROR(logger, fmt, ...) HAHA_LOG_FMT_LEVEL(logger, haha::LogLevel::Level::ERROR, fmt, __VA_ARGS__)
#define HAHA_LOG_FMT_FATAL(logger, fmt, ...)  HAHA_LOG_FMT_LEVEL(logger, haha::LogLevel::Level::FATAL, fmt, __VA_ARGS__)

#define HAHA_LOG_ROOT() haha::LoggerMgr::getInstance().getRoot()
#define HAHA_LOG_NAME(name) haha::LoggerMgr::getInstance().getLogger(name)

class Logger;

// 日志级别
class LogLevel{
public:
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* toString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string &str);
};

// 日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger,
            LogLevel::Level level, 
            const char *file, 
            int32_t line, 
            uint32_t elapse,
            uint32_t thread_id, 
            // uint32_t fiber_id, 
            uint64_t time,
            const std::string &thread_name);

    std::shared_ptr<Logger> getLogger() const {return m_logger;}
    const char * getFile() const {return m_file;}      
    int32_t getLine() const {return m_line;}         
    uint32_t getElapse() const {return m_elapse;}    
    uint32_t getThreadId() const {return m_threadId;}    
    // uint32_t getFiberId() const {return m_fiberId;}            
    uint64_t getTime() const {return m_time;}     
    const std::string& getThreadName() const { return m_threadName;}          
    std::string getContent() const {return m_ss.str();}  
    LogLevel::Level getLevel() const {return m_level;}

    std::stringstream& getSS() {return m_ss;}
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
       
private:
    std::shared_ptr<Logger> m_logger;   // 日志器
    const char * m_file = nullptr;      // 文件名
    int32_t m_line = 0;                 // 行号
    uint32_t m_elapse = 0;              // 启动到现在的毫秒数
    uint32_t m_threadId = 0;            // 线程ID
    // uint32_t m_fiberId = 0;             // 协程ID
    uint64_t m_time;                    // 时间戳
    std::string m_threadName;           // 线程名
    std::stringstream m_ss;             // 事件内容

    LogLevel::Level m_level;
};

// 日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string &pattern);
    std::string format(LogEvent::ptr event);

    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        // FormatItem(const std::string &fmt = ""){}
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };
    void init();

    bool isError() const {return m_error;}
    const std::string getPattern() const {return m_pattern;}

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};


// 日志输出地
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SpinLock MutexType;
    virtual ~LogAppender(){}

    virtual void log(LogEvent::ptr event) = 0;
    // virtual std::string toYamlString() = 0;

    LogFormatter::ptr getFormatter();
    void setFormatter(LogFormatter::ptr val);

    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}

    bool hasFormatter() { return m_formatter != nullptr; }
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger>{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef SpinLock MutexType;
    Logger(const std::string &name = "root");

    void log(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}
    std::string getName() const {return m_name;}

    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string &val);
    LogFormatter::ptr getFormatter();

    std::string toYamlString();

private:
    std::string m_name;                         // 日志名称
    LogLevel::Level m_level;                    // 日志级别
    MutexType m_mutex;
    std::list<LogAppender::ptr> m_appenders;    // Appender集合
    LogFormatter::ptr m_formatter;
    Logger::ptr m_root;
};

class LogEventWrap{
public:
    /**
     * @brief 构造函数
     * @param[in] logger 日志器 
     * @param[in] event 日志事件
     */
    LogEventWrap(Logger::ptr logger, LogEvent::ptr event);

    /**
     * @brief 析构函数
     * @details 日志事件在析构时由日志器进行输出
     */
    ~LogEventWrap();

    /**
     * @brief 获取日志事件
     */
    LogEvent::ptr getLogEvent() const { return m_event; }

    std::stringstream& getSS(){
        return m_event->getSS();
    }

private:
    /// 日志器
    Logger::ptr m_logger;
    /// 日志事件
    LogEvent::ptr m_event;
};


// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(LogEvent::ptr event) override;
    // virtual std::string toYamlString() override;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    virtual void log(LogEvent::ptr event) override;
    // virtual std::string toYamlString() override;
    
    // 重新打开文件，打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
    uint64_t m_lastTime;
};


class LoggerManager{
public:
    typedef SpinLock MutexType;
    LoggerManager();
    Logger::ptr getLogger(const std::string &name);
    void init();
    Logger::ptr getRoot() const {return m_root;}

    std::string toYamlString();
private:
    MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};


typedef Singleton<LoggerManager> LoggerMgr;

}

#endif