#ifndef __HAHA_LOGAPPENDER_H__
#define __HAHA_LOGAPPENDER_H__

#include <fstream>
#include <functional>
#include <atomic>
#include <iostream>
#include "base/Mutex.h"
#include "log/LogUtil.h"
#include "log/LogFile.h"
#include "log/LogStream.h"
#include "log/LogFormatter.h"
#include "base/ConditionVariable.h"

namespace haha{

namespace log{

// 日志输出地
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SpinLock MutexType;
    virtual ~LogAppender(){}

    // virtual void append(const LogInfo &info) = 0;
    virtual void append(const LogInfo &info) = 0;

    virtual void flush() = 0;
    // virtual std::string toYamlString() = 0;

    LogFormatter::ptr getFormatter();
    void setFormatter(LogFormatter::ptr val);

    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}

    bool hasFormatter() { return m_formatter != nullptr; }

    // 只有异步日志才需要
    virtual void setNotifyFunc(const std::function<void()> &notify_func) = 0;
    virtual bool empty() = 0;

protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;
};


class SyncLogAppender : public LogAppender{
public:
    virtual ~SyncLogAppender() {}
    void flush() {}
    void setNotifyFunc(const std::function<void()> &notify_func) override {}
    bool empty() { return true; }
};

class AsyncLogAppender : public LogAppender{
public:
    AsyncLogAppender();
                    // int flushInterval);
    
    virtual ~AsyncLogAppender() {}
    
    void setNotifyFunc(const std::function<void()> &notify_func) { notify_func_ = std::move(notify_func); }

    bool empty() { 
        MutexType::RAIILock lock(mutex_);
        return buffers_.empty(); 
    }

protected:
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    MutexType mutex_;

    // 输入缓冲区们
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;

    // 输出缓冲区们
    BufferPtr newBuffer1;
    BufferPtr newBuffer2;
    BufferVector buffersToWrite;

    // 唤醒日志线程
    std::function<void()> notify_func_;
};


// 输出到控制台的同步Appender
class StdoutSyncLogAppender : public SyncLogAppender{
public:
    typedef std::shared_ptr<StdoutSyncLogAppender> ptr;
    // void append(const LogInfo &info) override;
    void append(const LogInfo &info) override;
};

// 输出到文件的同步Appender
class FileSyncLogAppender : public SyncLogAppender{
public:
    typedef std::shared_ptr<FileSyncLogAppender> ptr;

    FileSyncLogAppender(const std::string &filepath,
                        off_t rollSize);
    
    // void append(const LogInfo &info) override;
    void append(const LogInfo &info) override;

private:
    std::string filepath_;
    const off_t rollSize_;
    // std::ofstream filestream_;
    std::unique_ptr<LogFile> file_;
    uint64_t lastTime_;
};

// // 输出到控制台的异步Appender
// class StdoutAsyncLogAppender : public AsyncLogAppender{
// public:
//     typedef std::shared_ptr<StdoutAsyncLogAppender> ptr;
    
//     StdoutAsyncLogAppender(off_t rollSize,
//                             int flushInterval);
//     void append(const LogInfo &info) override;
//     void flush() override;
//     // virtual std::string toYamlString() override;
// };

// 输出到文件的异步Appender
class FileAsyncLogAppender : public AsyncLogAppender{
public:
    typedef std::shared_ptr<FileAsyncLogAppender> ptr;

    FileAsyncLogAppender(const std::string &filepath,
                        off_t rollSize);
                        // int flushInterval);
    
    ~FileAsyncLogAppender(){ 
        file_->flush(); 
    }

    // void append(const LogInfo &info) override;
    void append(const LogInfo &info) override;
    void flush() override;
    // virtual std::string toYamlString() override;
private:
    std::string filepath_;
    const off_t rollSize_;
    std::unique_ptr<LogFile> file_;
    uint64_t lastTime_;
};

}

}

#endif