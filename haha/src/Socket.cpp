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
    }
    else if(fdtype == FDTYPE::NONBLOCK){
        fd_ = sockops::createNBSocket();
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

std::pair<int, InetAddress> Socket::accept() {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int connfd = ::accept(fd_, (sockaddr *)&addr, &len);
    InetAddress peer(addr);
    return std::make_pair(connfd, peer);
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

}