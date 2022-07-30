#include "base/FileUtil.h"
#include <exception>
#include <assert.h>

namespace haha{

namespace FileUtil{

SmallFileUtil::SmallFileUtil(const char* filename)
    :fd_(::open(filename, O_RDONLY | O_CLOEXEC)),
    err_(0)
{
    buf_[0] = '\0';
    if (fd_ < 0)
    {
        err_ = errno;
    }
}

SmallFileUtil::~SmallFileUtil()
{
    if (fd_ >= 0)
    {
        ::close(fd_); // FIXME: check EINTR
    }
}

// return errno
int SmallFileUtil::readToString(int maxSize,
                                std::string &content,
                                int64_t* fileSize,
                                int64_t* modifyTime,
                                int64_t* createTime)
{
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");

    int err = err_;
    if (fd_ >= 0)
    {
        content.clear();

        if (fileSize)
        {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0)
            {
                if (S_ISREG(statbuf.st_mode))
                {
                    *fileSize = statbuf.st_size;
                    content.reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
                }
                else if (S_ISDIR(statbuf.st_mode))
                {
                    err = EISDIR;
                }
                if (modifyTime)
                {
                    *modifyTime = statbuf.st_mtime;
                }
                if (createTime)
                {
                    *createTime = statbuf.st_ctime;
                }
            }
            else
            {
                err = errno;
            }
        }

        while (content.size() < static_cast<size_t>(maxSize))
        {
            size_t toRead = std::min(static_cast<size_t>(maxSize) - content.size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);
            if (n > 0)
            {
                content.append(buf_, n);
            }
            else
            {
                if (n < 0)
                {
                err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int SmallFileUtil::readToBuffer(int* size)
{
    int err = err_;
    if (fd_ >= 0)
    {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1, 0);
        if (n >= 0)
        {
            if (size)
            {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        }
        else
        {
            err = errno;
        }
    }
    return err;
}


int readSmallFile(const char* filename,
                    int maxSize,
                    std::string &content,
                    int64_t* fileSize,
                    int64_t* modifyTime,
                    int64_t* createTime)
{
    SmallFileUtil file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}


File::File(const char* file_path)
    :filePath_(file_path)
    ,fd_(-1)
    ,size_(std::filesystem::file_size(file_path))
    ,isOpen_(false)
    ,isClose_(true){
}


File::STATE File::open(OPEN_MOD mod){
    if(isOpen_){
        return STATE::SUCCESS;
    }
    switch (mod)
    {
    case READ_ONLY:
        fd_ = ::open(filePath_.c_str(), O_RDONLY);
        break;
    case WRITE_ONLY:
        fd_ = ::open(filePath_.c_str(), O_WRONLY);
    case WRITE_APPEND:
        fd_ = ::open(filePath_.c_str(), O_WRONLY | O_APPEND);
    default:
        return STATE::FAIL;
        break;
    }
    if(fd_ == -1){
        return STATE::FAIL;
    }
    isOpen_ = true;
    isClose_ = false;
    return STATE::SUCCESS;
}

void File::close(){
    if(isOpen_){
        ::close(fd_);
    }
    isOpen_ = false;
    isClose_ = true;
}


/* ============================================================FileSendStream============================================================ */


FileSendStream::FileSendStream(const char* file_path, int outfd, bool isBlock, SEND_MOD mod)
    :file_(std::make_shared<File>(file_path))
    ,outfd_(outfd)
    ,isBlock_(isBlock)
    ,mod_(mod)
    ,mmap_(nullptr)
    ,sendFile_(nullptr)
    ,sended_bytes_(0){
    
    file_->open(File::OPEN_MOD::READ_ONLY);

    if(mod == MMAP){
        mmap_ = std::make_shared<MMap>(file_->getFd());
        remain_bytes_ = mmap_->getSize();
    }
    else{
        sendFile_ = std::make_shared<SendFile>(file_->getFd(), outfd_);
        remain_bytes_ = sendFile_->getSize();
    }
}

FileSendStream::FileSendStream(File::ptr file, int outfd, bool isBlock, SEND_MOD mod)
    :file_(file)
    ,outfd_(outfd)
    ,isBlock_(isBlock)
    ,mod_(mod)
    ,mmap_(nullptr)
    ,sendFile_(nullptr)
    ,sended_bytes_(0){

    file_->open(File::OPEN_MOD::READ_ONLY);

    if(mod == MMAP){
        mmap_ = std::make_shared<MMap>(file_->getFd());
        remain_bytes_ = mmap_->getSize();
    }
    else{
        sendFile_ = std::make_shared<SendFile>(file_->getFd(), outfd_);
        remain_bytes_ = sendFile_->getSize();
    }
}

int FileSendStream::send(int *lastLen){
    int n = 0;
    int ret;
    if(mmap_){
        do{
            auto addr = mmap_->getAddress() + sended_bytes_;
            ret =  write(outfd_, addr, remain_bytes_);
            if(ret <= 0){
                break;
            }
            n += ret;
            remain_bytes_ -= ret;
            sended_bytes_ += ret;
        }while(sendable() && !isBlock_);
    }
    else{
        do{
            ret = sendFile_->send(remain_bytes_);
            if(ret <= 0){
                break;
            }
            n += ret;
            remain_bytes_ -= ret;
            sended_bytes_ += ret;
        }while(sendable() && !isBlock_);
    }

    if(lastLen){
        *lastLen = ret;
    }
    return n;
}


/* ============================================================FileSSLSendStream============================================================ */


FileSSLSendStream::FileSSLSendStream(const char* file_path, SSL *ssl, bool isBlock)
    :FileSendStream(file_path, SSL_get_fd(ssl), isBlock, SEND_MOD::MMAP)
{

}


FileSSLSendStream::FileSSLSendStream(File::ptr file, SSL *ssl, bool isBlock)
    :FileSendStream(file, SSL_get_fd(ssl), isBlock, SEND_MOD::MMAP)
{

}


int FileSSLSendStream::send(int *lastLen){
    int n = 0;
    int ret = 0;
    if(mmap_){
        do{
            auto addr = mmap_->getAddress() + sended_bytes_;
            ret = SSL_write(ssl_, addr, remain_bytes_);
            if(ret <= 0){
                break;
            }
            n += ret;
            remain_bytes_ -= ret;
            sended_bytes_ += ret;
        }while(sendable() && !isBlock_);
    }

    if(lastLen){
        *lastLen = ret;
    }
    return n;
}

}

}