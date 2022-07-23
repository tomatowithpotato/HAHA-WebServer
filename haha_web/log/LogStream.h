#ifndef __HAHA_LOG_LOGSTREAM_H__
#define __HAHA_LOG_LOGSTREAM_H__

#include <string>
#include <string.h>
#include "base/noncopyable.h"
#include "log/LogUtil.h"

namespace haha{

namespace log{

class LogStream{
typedef LogStream self;
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    self& operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);

    self& operator<<(const void*);

    self& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    self& operator<<(double);
    // self& operator<<(long double);

    self& operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }

    // self& operator<<(signed char);
    // self& operator<<(unsigned char);

    self& operator<<(const char* str)
    {
        if (str)
        {
        buffer_.append(str, strlen(str));
        }
        else
        {
        buffer_.append("(null)", 6);
        }
        return *this;
    }

    self& operator<<(const unsigned char* str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    self& operator<<(const std::string& v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

    self& operator<<(const std::string_view& v)
    {
        buffer_.append(v.data(), v.size());
        return *this;
    }

    self& operator<<(const Buffer& v)
    {
        *this << v.toStringView();
        return *this;
    }

    void append(const char* data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 48;
};

}

}

#endif