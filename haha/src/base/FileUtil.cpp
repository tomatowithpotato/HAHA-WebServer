#include "base/FileUtil.h"
#include <exception>

namespace haha{

namespace FileUtil{

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


FileSender::FileSender(const char* file_path, int outfd, bool isBlock, SEND_MOD mod)
    :file_(std::make_shared<File>(file_path))
    ,outfd_(outfd)
    ,isBlock_(isBlock)
    ,sended_bytes_(0)
    ,mmap_(nullptr)
    ,sendFile_(nullptr)
    ,mod_(mod){
    
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

FileSender::FileSender(File::ptr file, int outfd, bool isBlock, SEND_MOD mod)
    :file_(file)
    ,outfd_(outfd)
    ,isBlock_(isBlock)
    ,sended_bytes_(0)
    ,mmap_(nullptr)
    ,sendFile_(nullptr)
    ,mod_(mod){

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

int FileSender::send(int *lastLen){
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
        }while(sendable() && !isBlock_);
    }
    else{
        do{
            ret = sendFile_->send(remain_bytes_);
            if(ret <= 0){
                break;
            }
            n += ret;
        }while(sendable() && !isBlock_);
    }
    remain_bytes_ -= n;
    sended_bytes_ += n;
    if(lastLen){
        *lastLen = ret;
    }
    return n;
}

int FileSSLSender::send(int *lastLen){
    int n;
    return n;
}

}

}