#ifndef __HAHA_LOG_LOGUTIL_H__
#define __HAHA_LOG_LOGUTIL_H__

#include <string>
#include <string.h>
#include "base/noncopyable.h"
#include "base/ConditionVariable.h"

namespace haha{

namespace log{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;
const int kbuffersMaxSize = 25;

template<int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer()
        : cur_(data_)
    {
    }

    void append(const char* /*restrict*/ buf, size_t len)
    {
        // FIXME: append partially
        if (static_cast<size_t>(avail()) > len)
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    // write to data_ directly
    char* current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero() { ::bzero(data_, sizeof(data_)); }

    // for used by unit test
    std::string toString() const { return string(data_, length()); }
    std::string_view toStringView() const { return std::string_view(data_, length()); }

private:
    const char* end() const { return data_ + sizeof(data_); }

    void (*cookie_)();
    char data_[SIZE];
    char* cur_;
};


// compile time calculation of basename of source file
class SourceFile
{
public:
    template<int N>
    SourceFile(const char (&arr)[N])
        : data_(arr),
        size_(N-1)
    {
        const char* slash = strrchr(data_, '/'); // builtin function
        if (slash)
        {
            data_ = slash + 1;
            size_ -= static_cast<int>(data_ - arr);
        }
    }

    explicit SourceFile(const char* filename)
        : data_(filename)
    {
        const char* slash = strrchr(filename, '/');
        if (slash)
        {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
};

}

}

#endif