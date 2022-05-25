#ifndef __HAHA_SOCKET_H__
#define __HAHA_SOCKET_H__

#include "InetAddress.h"
#include <fcntl.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

namespace haha{

namespace sockops{

/*创建非阻塞socket*/
int createNBSocket();
/*创建默认（阻塞）socket*/
int createSocket();
void setNonBlocking(int fd);
/*禁用Nagle算法*/
void setNoDelay(int fd);
/*获取socket对端地址*/
InetAddress getPeerName(int fd);
/*获取socket本端地址*/
InetAddress getSockName(int fd);

}

class Socket{
public:
    enum FDTYPE{
        BLOCK,
        NONBLOCK,
    };

    explicit Socket(int fd):fd_(fd) {}
    explicit Socket(FDTYPE fdtype);
    ~Socket();
    int getFd() const noexcept { return fd_; }
    
    void bind(const InetAddress &address);
    void listen();
    // 接受连接
    std::pair<int, InetAddress> accept();
    // 开启地址复用
    void enableReuseAddr(bool on);
    // 开启端口复用
    void enableReusePort(bool on);
    // 开启保活
    void enableKeepAlive(bool on);

private:
    int fd_;
};

}

#endif