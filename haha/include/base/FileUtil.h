#ifndef __HAHA_FILEUTIL_H__
#define __HAHA_FILEUTIL_H__

#include <memory>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace haha{

namespace FileUtil{

class File{
public:
    typedef std::shared_ptr<File> ptr;
    enum STATE{
        SUCCESS,
        FAIL,
    };
    enum OPEN_MOD{
        READ_ONLY,
        WRITE_ONLY,
        WRITE_APPEND,
        READ_WRITE,
    };
    File(const char* file_path);
    ~File() { close(); }
    STATE open(OPEN_MOD);
    void close();
    int getFd() const { return fd_; }
    size_t getSize() const { return size_; }
    bool isOpen() const { return isOpen_; }
    bool isClose() const { return isClose_; }
private:
    std::string filePath_;
    int fd_;
    size_t size_;
    bool isOpen_;
    bool isClose_;
};


class MMap {
public:
    typedef std::shared_ptr<MMap> ptr;
    explicit MMap(int infd) : fd_(infd){
        struct stat st;
        ::fstat(infd, &st);
        size_ = st.st_size;
        addr_ = ::mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
        if (addr_ == MAP_FAILED) {
            addr_ = nullptr;
            size_ = 0;
        }
    }
    ~MMap() {
        if (addr_){
            ::munmap(addr_, size_);
        }
        addr_ = nullptr;
    }
    char *getAddress() const noexcept { return static_cast<char *>(addr_); }
    int getFd() const noexcept { return fd_; }
    size_t getSize() const noexcept { return size_; }

private:
    void *addr_;
    int fd_;
    size_t size_;
};


class SendFile {
public:
    typedef std::shared_ptr<SendFile> ptr;
    explicit SendFile(int infd, int outfd)
        : infd_(infd), outfd_(outfd){
        struct stat st;
        ::fstat(infd, &st);
        size_ = st.st_size;
    }

    int send(off_t *offset, size_t size) {
        // If offset is not NULL, then it points to a variable holding the file offset from which sendfile() will start reading data from in_fd.  
        // When sendfile() returns, this variable will be set to the offset of the byte following the last byte that was read.  
        // If offset is not NULL, then sendfile() does not modify the file offset of in_fd; 
        // otherwise the file offset is adjusted to reflect the number of bytes read from in_fd.
        return ::sendfile(outfd_, infd_, offset, size);
    }

    int send(size_t size) {
        // If offset is NULL, then data will be read from in_fd starting at the
        // file offset, and the file offset will be updated by the call.
        return ::sendfile(outfd_, infd_, nullptr, size);
    }

    int getInFd() const noexcept { return infd_; }
    int getOutFd() const noexcept { return outfd_; }
    size_t getSize() const noexcept { return size_; }

private:
    int infd_;
    int outfd_;
    size_t size_;
};


class FileSendStream{
public:
    typedef std::shared_ptr<FileSendStream> ptr;
    enum SEND_MOD{
        SENDFILE,
        MMAP,
    };
    explicit FileSendStream(const char* file_path, int outfd, bool isBlock, SEND_MOD mod = MMAP);
    explicit FileSendStream(File::ptr file, int outfd, bool isBlock, SEND_MOD mod = MMAP);
    bool sendable() { return remain_bytes_ == 0; }
    virtual int send(int *lastLen = nullptr);
    size_t remainBytes() { return remain_bytes_; }
    size_t sendedBytes() { return sended_bytes_; }
private:
    File::ptr file_;
    int outfd_;
    bool isBlock_;
    MMap::ptr mmap_;
    SEND_MOD mod_;
    SendFile::ptr sendFile_;
    size_t remain_bytes_;
    size_t sended_bytes_;
};


class FileSSLSendStream : FileSendStream{
    int send(int *lastLen = nullptr) override;
};

}

}

#endif