#ifndef __HAHA_TIMESTAMP_H__
#define __HAHA_TIMESTAMP_H__

#include <limits>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <chrono>
#include <iomanip>

namespace haha{

class TimeStamp {
public:
    static const int MicroSecondsPerSecond = 1000 * 1000;
    static const int MicroSecondsPerMillSecond = 1000;

    explicit TimeStamp(uint64_t microsecond = 0) : microsecond_(microsecond) {}
    /*秒*/
    uint64_t second() const noexcept { return microsecond_ / MicroSecondsPerSecond; }
    /*毫秒*/
    uint64_t millsecond() const noexcept { return microsecond_ / MicroSecondsPerMillSecond; }
    /*微秒*/
    uint64_t microsecond() const noexcept { return microsecond_; }
    /*纳秒*/
    uint64_t nanosecond() const noexcept { return microsecond_ * 1000; }

    TimeStamp &operator+(const TimeStamp &t) noexcept {
        microsecond_ += t.microsecond_;
        return *this;
    }

    TimeStamp &operator-(const TimeStamp &t) noexcept {
        microsecond_ -= t.microsecond_;
        return *this;
    }

    TimeStamp &operator+(int second) noexcept {
        microsecond_ += (second * 1000000);
        return *this;
    }

    TimeStamp &operator-(int second) noexcept {
        microsecond_ -= (second * 1000000);
        return *this;
    }

    bool operator<(const TimeStamp &t) const noexcept {
        return microsecond_ < t.microsecond_;
    }

    bool operator<=(const TimeStamp &t) const noexcept {
        return microsecond_ <= t.microsecond_;
    }

    bool operator>(const TimeStamp &t) const noexcept {
        return microsecond_ > t.microsecond_;
    }
    bool operator==(const TimeStamp &t) const noexcept {
        return microsecond_ == t.microsecond_;
    }

    static TimeStamp now() {
        timeval tv;
        ::gettimeofday(&tv, nullptr);
        uint64_t microsecond = tv.tv_sec * 1000000 + tv.tv_usec;
        return TimeStamp(microsecond);
    }

    static TimeStamp second(int sec) { return TimeStamp(sec * 1000000); }
    static TimeStamp millsecond(int msec) { return TimeStamp(msec * 1000); }
    static TimeStamp microsecond(int microsec) { return TimeStamp(microsec); }
    static TimeStamp nanosecond(int nsec) { return TimeStamp(nsec / 1000); }
    static TimeStamp max() noexcept {
        return TimeStamp(std::numeric_limits<uint64_t>::max());
    }

    static TimeStamp nowSecond(int sec) { return TimeStamp::now() + second(sec); }
    static TimeStamp nowMsecond(int msec) {
        return TimeStamp::now() + millsecond(msec);
    }
    static TimeStamp nowNanosecond(int nsec) {
        return TimeStamp::now() + nanosecond(nsec);
    }

    static std::string getDate() {
        char buffer[50]{0};
        time_t t = ::time(nullptr);
        ::strftime(buffer, 50, "%a, %d %b %Y %H:%M:%S GMT", ::gmtime(&t));
        return buffer;
    }

    std::string toFormattedString(bool showMicroseconds = true) const {
        char buf[64] = {0};
        time_t seconds = static_cast<time_t>(second());
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time);

        if (showMicroseconds)
        {
            int microseconds = static_cast<int>(microsecond());
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                    microseconds);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        }
        return buf;
    }

    static time_t get_TZoffset(){
        auto now = std::time(nullptr);
        auto const tm = *std::localtime(&now);
        std::ostringstream os;
        os << std::put_time(&tm, "%z");
        std::string s = os.str();
        // s is in ISO 8601 format: "±HHMM"
        int h = std::stoi(s.substr(0,3), nullptr, 10);
        int m = std::stoi(s[0]+s.substr(3), nullptr, 10);

        return h * 3600 + m * 60;
    }

private:
    uint64_t microsecond_;
};

}

#endif