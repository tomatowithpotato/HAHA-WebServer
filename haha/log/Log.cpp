#include "log/Log.h"

namespace haha{

/* *************************************各种FormatItem************************************* */

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << LogLevel::toString(event->getLevel());
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    ElapseFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << event->getThreadId();
    }
};


// class FiberIdFormatItem : public LogFormatter::FormatItem{
// public:
//     FiberIdFormatItem(const std::string &fmt = ""){}
//     void format(std::ostream &os, LogEvent::ptr event) override{
//         os << event->getFiberId();
//     }
// };

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, LogEvent::ptr event) override {
        os << event->getThreadName();
    }
};


class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S"):m_format(format){
        if(m_format.empty()){
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::ostream &os, LogEvent::ptr event) override{
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};


class FilenameFormatItem : public LogFormatter::FormatItem{
public:
    FilenameFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string &fmt = ""){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string &str):m_string(str){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string &str = ""):m_string(str){}
    void format(std::ostream &os, LogEvent::ptr event) override{
        os << "\t";
    }
private:
    std::string m_string;
};

/* *************************************LogLevel************************************* */

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

/* *************************************LogEventWrap************************************* */

LogEventWrap::LogEventWrap(Logger::ptr logger, LogEvent::ptr event)
    :m_logger(logger)
    ,m_event(event){
}

LogEventWrap::~LogEventWrap(){
    m_logger->log(m_event);
}

/* *************************************LogEvent************************************* */

LogEvent::LogEvent(std::shared_ptr<Logger> logger, 
                    LogLevel::Level level, 
                    const char *file, 
                    int32_t line, 
                    uint32_t elapse,
                    uint32_t thread_id, 
                    // uint32_t fiber_id, 
                    uint64_t time,
                    const std::string &thread_name)
        :m_logger(logger),
        m_file(file), 
        m_line(line), 
        m_elapse(elapse), 
        m_threadId(thread_id), 
        // m_fiberId(fiber_id), 
        m_time(time),
        m_threadName(thread_name),
        m_level(level){
    
}

void LogEvent::format(const char* fmt, ...){
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al){
    char *buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1){
        m_ss << std::string(buf, len);
        free(buf);
    }
}


/* *************************************Logger************************************* */

Logger::Logger(const std::string &name)
    :m_name(name),
    m_level(LogLevel::DEBUG){
    // m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%T[%p]%T[%c]%T%f:%l%T%m%n"));
}


void Logger::setFormatter(LogFormatter::ptr val){
    MutexType::RAIILock lock(m_mutex);
    m_formatter = val;

    for(auto &i : m_appenders){
        MutexType::RAIILock ll(i->m_mutex);
        // appender还没设置格式，因此把logger的默认格式赋给它
        if(!i->hasFormatter()){
            i->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string &val){
    LogFormatter::ptr new_val(new LogFormatter(val));
    if(new_val->isError()){
        std::cout << "Logger setFormatter name=" << m_name
                  << " value=" << val << " invalid formatter"
                  << std::endl;
        return;
    }
    // m_formatter = new_val;
    setFormatter(new_val);
}

LogFormatter::ptr Logger::getFormatter(){
    MutexType::RAIILock lock(m_mutex);
    return m_formatter;
}

void Logger::addAppender(LogAppender::ptr appender){
    MutexType::RAIILock lock(m_mutex);
    if(!appender->getFormatter()){
        MutexType::RAIILock ll(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
    MutexType::RAIILock lock(m_mutex);
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders(){
    m_appenders.clear();
}

void Logger::log(LogEvent::ptr event){
    auto level = event->getLevel();
    if(level >= m_level){
        auto self = shared_from_this();
        MutexType::RAIILock lock(m_mutex);
        if(!m_appenders.empty()){
            for(auto &i : m_appenders){
                i->log(event);
            }
        }
        else if(m_root){
            m_root->log(event);
        }
    }
}


/* *************************************LogAppender************************************* */

void LogAppender::setFormatter(LogFormatter::ptr val){
    MutexType::RAIILock lock(m_mutex);
    m_formatter = val;
}

LogFormatter::ptr LogAppender::getFormatter(){
    MutexType::RAIILock lock(m_mutex);
    return m_formatter;
}

FileLogAppender::FileLogAppender(const std::string &filename)
    :m_filename(filename){
    reopen();
}

void FileLogAppender::log(LogEvent::ptr event){
    auto level = event->getLevel();
    if(level >= m_level){
        // 每秒都尝试重新打开文件
        uint64_t now = time(0);
        if(now != m_lastTime){
            reopen();
            m_lastTime = now;
        }
        MutexType::RAIILock lock(m_mutex);
        if(!(m_filestream << m_formatter->format(event))){
            std::cout << "error" << std::endl;
        }
    }
}

bool FileLogAppender::reopen(){
    MutexType::RAIILock lock(m_mutex);
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !m_filestream.fail();
}

void StdoutLogAppender::log(LogEvent::ptr event){
    auto level = event->getLevel();
    if(level >= m_level){
        MutexType::RAIILock lock(m_mutex);
        std::cout << m_formatter->format(event);
    }
}

/* *************************************LogFormatter 重量级************************************* */

LogFormatter::LogFormatter(const std::string &pattern)
    :m_pattern(pattern){
    init();
}


// %xxx %xxx{xxx} %%
void LogFormatter::init(){
    // str, format, type
    // type 0 普通字符串 1 格式化字符串
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr; // 普通字符串
    for(size_t i = 0; i < m_pattern.size(); i++){
        if(m_pattern[i] != '%'){
            nstr.append(1, m_pattern[i]);
            continue;
        }
        // %% -> %
        if(i + 1 < m_pattern.size()){
            if(m_pattern[i+1] == '%'){
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str; // % 到 { 之间的字符串
        std::string fmt; // { 到 } 之间的字符串
        while(n < m_pattern.size()){
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')){
                str = m_pattern.substr(i+1, n-i-1);
                break;
            }
            if(fmt_status == 0){
                if(m_pattern[n] == '{'){
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1; // 解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }    
            }
            else if(fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()){
                if(str.empty()){
                    str = m_pattern.substr(i+1);
                }
            }
        }

        if(fmt_status == 0){
            // 无格式普通字符串
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            // 格式字符串
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
        // 未转到状态0，解析出错
        else if(fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    if(!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, std::string(), 0));
    }

    // %m -- 消息体
    // %p -- level
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程id
    // %n -- 回车换行
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号
    // %T -- tab

    // 麻了, 骚啊
    // 建立格式符到相应的格式处理器的映射
    static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string &fmt){return FormatItem::ptr(new C(fmt));}}
        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        // XX(F, FiberIdFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };
    for(auto &i : vec){
        // 普通字符串 直接放入
        if(std::get<2>(i) == 0){
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else{
            // 格式字符串 将格式符映射为对应的格式处理器
            auto it = s_format_items.find(std::get<0>(i));
            // 找不到就加报错信息
            if(it == s_format_items.end()){
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            }
            else{
                // 找到就加入
                m_items.push_back(FormatItem::ptr(it->second(std::get<1>(i))));
            }
        }
        // // 控制台打印
        // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
}


std::string LogFormatter::format(LogEvent::ptr event){
    std::stringstream ss;
    for(auto &i : m_items){
        i->format(ss, event);
    }
    return ss.str();
}


/* *************************************LoggerManager************************************* */

LoggerManager::LoggerManager(){
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

    m_loggers[m_root->m_name] = m_root;

    init();
}

Logger::ptr LoggerManager::getLogger(const std::string &name){
    MutexType::RAIILock lock(m_mutex);
    auto it = m_loggers.find(name);
    if(it != m_loggers.end()){
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

void LoggerManager::init(){

}

}