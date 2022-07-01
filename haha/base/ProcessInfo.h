#ifndef __HAHA_PROCESSINFO_H__
#define __HAHA_PROCESSINFO_H__

#include <string>
#include <vector>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include "base/FileUtil.h"
#include "base/Thread.h"
#include "base/TimeStamp.h"

namespace haha{

namespace ProcessInfo
{
    pid_t pid();
    std::string pidString();
    uid_t uid();
    std::string username();
    uid_t euid();
    TimeStamp startTime();
    int clockTicksPerSecond();
    int pageSize();
    bool isDebugBuild();  // constexpr

    std::string hostname();
    std::string procname();
    std::string_view procname(const std::string& stat);

    /// read /proc/self/status
    std::string procStatus();

    /// read /proc/self/stat
    std::string procStat();

    /// read /proc/self/task/tid/stat
    std::string threadStat();

    /// readlink /proc/self/exe
    std::string exePath();

    int openedFiles();
    int maxOpenFiles();

    struct CpuTime
    {
        double userSeconds;
        double systemSeconds;

        CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }

        double total() const { return userSeconds + systemSeconds; }
    };
    CpuTime cpuTime();

    int numThreads();
    std::vector<pid_t> threads();
}  // namespace ProcessInfo

}

#endif