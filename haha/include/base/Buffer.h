#ifndef __HAHA_BUFFER_H__
#define __HAHA_BUFFER_H__

#include <string>
#include <vector>
#include <atomic>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <assert.h>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace haha{

class Buffer{
/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode

public:
    typedef std::shared_ptr<Buffer> ptr;
    // static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer(size_t initialSize = kInitialSize);
    ~Buffer() = default;
    // 可写字节数
    size_t WritableBytes() const; 
    // 可读字节数      
    size_t ReadableBytes() const ;
    // 头部预留字节数
    size_t PrependableBytes() const;
    // 数据可读处
    const char* Peek() const;

    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);

    void RetrieveAll() ;
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

private:
    char* BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSpace_(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;
    std::atomic<std::size_t> writePos_;
};

}

#endif