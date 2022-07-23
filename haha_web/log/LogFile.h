#ifndef __HAHA_LOG_LOGFILE_H__
#define __HAHA_LOG_LOGFILE_H__

#include <string>
#include <memory>
#include "base/noncopyable.h"
#include "base/Mutex.h"

namespace haha{

namespace log{

class AppendFile : noncopyable
{
public:
    AppendFile(const char* filename);

    ~AppendFile();

    void append(const char* logline, size_t len);

    void flush();

    off_t writtenBytes() const { return writtenBytes_; }

private:

    size_t write(const char* logline, size_t len);

    FILE* fp_;
    char buffer_[64*1024];
    off_t writtenBytes_;
};


class LogFile : noncopyable
{
public:
    typedef std::shared_ptr<LogFile> ptr;
    typedef MutexLock MutexType;
    LogFile(const std::string& basename,
            off_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);

    static std::string getLogFileName(const std::string& basename, time_t* now);

    const std::string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    MutexType mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    std::unique_ptr<AppendFile> file_;

    const static int kRollPerSeconds_ = 60*60*24;
};

}

}

#endif