#include "log/LogInfo.h"
#include <stdarg.h>

namespace haha{

namespace log{

const char* LogLevel::toString(LogLevel::Level level){
// 骚操作时间
    switch (level)
    {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
        break;
    }
    return "UNKNOW";
}

LogLevel::Level LogLevel::FromString(const std::string &str){
#define XX(level, v) \
    if(str == #v){ \
        return LogLevel::level; \
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
#undef XX
    return LogLevel::UNKNOW;
}


LogInfo::LogInfo(const std::string &loggerName, 
                    LogLevel::Level level, 
                    const char *file, 
                    int32_t line, 
                    uint32_t elapse,
                    uint32_t thread_id, 
                    // uint32_t fiber_id, 
                    const TimePoint& time)
                    // const std::string &thread_name)
        :loggerName_(loggerName),
        file_(file), 
        line_(line), 
        elapse_(elapse), 
        threadId_(thread_id), 
        // m_fiberId(fiber_id), 
        time_(time),
        // threadName_(thread_name),
        level_(level)
{    
}

void LogInfo::format(const char* fmt, ...){
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogInfo::format(const char* fmt, va_list al){
    char *buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1){
        ss_.append(buf, len);
        free(buf);
    }
}


}

}