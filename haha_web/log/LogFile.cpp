#include "log/LogFile.h"
#include "base/ProcessInfo.h"
#include <assert.h>
#include <string.h>

namespace haha{

namespace log{

thread_local char t_errnobuf[512];
thread_local char t_time[64];
thread_local time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

AppendFile::AppendFile(const char* filename)
    :fp_(::fopen(filename, "ae")),  // 'e' for O_CLOEXEC
    writtenBytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));
    // posix_fadvise POSIX_FADV_DONTNEED ?
}

AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
    size_t written = 0;

    while (written != len)
    {
        size_t remain = len - written;
        size_t n = write(logline + written, remain);
        if (n != remain)
        {
        int err = ferror(fp_);
        if (err)
        {
            fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
            break;
        }
        }
        written += n;
    }

    writtenBytes_ += written;
}

void AppendFile::flush()
{
    ::fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
    // #undef fwrite_unlocked
    return ::fwrite_unlocked(logline, 1, len, fp_);
}

LogFile::LogFile(const std::string& basename,
                 off_t rollSize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN)
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    checkEveryN_(checkEveryN),
    count_(0),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)
{
    assert(basename.find('/') == std::string::npos);
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char* logline, int len)
{
    MutexType::RAIILock lock(mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush()
{
    MutexType::RAIILock lock(mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len)
{
    file_->append(logline, len);

    if (file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        ++count_;
        if (count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPeriod_ != startOfPeriod_)
            {
                rollFile();
            }
            else if (now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }
}

bool LogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > lastRoll_)
    {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new AppendFile(filename.c_str()));
        return true;
    }
    return false;
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm); // FIXME: localtime_r ?
    strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();

    char pidbuf[32];
    snprintf(pidbuf, sizeof(pidbuf), ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}

}

}