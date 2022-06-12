#include "Socket.h"

namespace haha{

void sockops::setNonBlocking(int fd) {
    int old = ::fcntl(fd, F_GETFL, 0);
    old |= O_NONBLOCK;
    ::fcntl(fd, F_SETFL, old);
}

void sockops::setNoDelay(int fd) {
    int opt = 1;
    ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

int sockops::createSocket() {
    return ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC,
                  IPPROTO_TCP);
}

int sockops::createNBSocket() {
  return ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                  IPPROTO_TCP);
}

InetAddress sockops::getPeerName(int fd) {
    sockaddr_in sockaddrIn;
    socklen_t len = sizeof(sockaddrIn);
    ::getpeername(fd, (sockaddr *)&sockaddrIn, &len);
    InetAddress address(sockaddrIn);
    return address;
}

InetAddress sockops::getSockName(int fd) {
    sockaddr_in sockaddrIn;
    socklen_t len = sizeof(sockaddrIn);
    ::getsockname(fd, (sockaddr *)&sockaddrIn, &len);
    InetAddress address(sockaddrIn);
    return address;
}


Socket::Socket(FDTYPE fdtype){
    if(fdtype == FDTYPE::BLOCK){
        fd_ = sockops::createSocket();
        isBlocked_ = true;
    }
    else if(fdtype == FDTYPE::NONBLOCK){
        fd_ = sockops::createNBSocket();
        isBlocked_ = false;
    }
}

Socket::~Socket() { ::close(fd_); }

void Socket::bind(const InetAddress &address) {
    if (::bind(fd_, address.getSockAddr(), sizeof(sockaddr_in)) < 0)
        ::exit(-1);
}

void Socket::listen() {
    if (::listen(fd_, SOMAXCONN) < 0)
        ::exit(-1);
}

Socket::ptr Socket::accept() {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int connfd = ::accept(fd_, (sockaddr *)&addr, &len);
    if(connfd <= 0){
        return nullptr;
    }
    return std::make_shared<Socket>(connfd);
}

void Socket::setNonBlocking(){
    isBlocked_ = false;
    sockops::setNonBlocking(fd_);
}

void Socket::enableReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::enableReusePort(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::enableKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

InetAddress Socket::getLocalAddress(){
    return sockops::getSockName(fd_);
}

InetAddress Socket::getRemoteAddress(){
    return sockops::getPeerName(fd_);
}

int Socket::send(Buffer::ptr buff, int *lastLen){
    int sendedBytes = 0;
    int saveErrno;
    int len;
    do {
        len = buff->WriteFd(fd_, &saveErrno);
        if (len <= 0) {
            break;
        }
        sendedBytes += len;
    } while (!isBlocked_ && buff->ReadableBytes());
    if(lastLen){
        *lastLen = len;
    }
    return sendedBytes;
}

int Socket::recv(Buffer::ptr buff, int *lastLen){
    int receivedBytes = 0;
    int saveErrno;
    int len;
    do {
        len = buff->ReadFd(fd_, &saveErrno);
        if (len <= 0) {
            break;
        }
        receivedBytes += len;
    } while (!isBlocked_);
    if(lastLen){
        *lastLen = len;
    }
    return receivedBytes;
}

}