#ifndef __HAHA_TCPSERVER_H__
#define __HAHA_TCPSERVER_H__

#include <memory>
#include <unordered_map>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "log/Log.h"
#include "config/config.h"
#include "base/Mutex.h"
#include "base/ReadWriteLock.h"
#include "InetAddress.h"
#include "Socket.h"
#include "EventLoopThreadPool.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"

namespace haha{

class TcpServer{

public:
    typedef std::unordered_map<int, std::shared_ptr<TcpConnection>> ConnectMap;
    // typedef CASLock MutexType;
    typedef SpinLock MutexType;

    enum MESSAGE_STATUS{
        OK,
        AGAIN,
    };

    TcpServer();
    void start(const InetAddress &address);
    void start();

    EventLoop::ptr getMainLoop() { return mainLoop_; }

    bool isHttps() const { return enable_https; }

private:
    void handleServerAccept();
    void handleRead(int fd);
    void handleWrite(int fd);
    void handleClose(int fd);
    void handleTimeout(int fd);
    void handleWakeup();
    void handleRead();
    void handleWrite();
    void handleClose();

    /* 因为函数都会被注册为回调函数，因此就不能持有shared_ptr，不然无法释放 */

    void handleConnectionRead(TcpConnection::weak_ptr);
    void handleConnectionWrite(TcpConnection::weak_ptr);
    void handleConnectionClose(TcpConnection::weak_ptr);

    void onSend(TcpConnection::weak_ptr);
    void onRecv(TcpConnection::weak_ptr);
    void onConnect(TcpConnection::weak_ptr);

protected:
    virtual MESSAGE_STATUS onMessage(TcpConnection::ptr);
    virtual bool onCreateConnection(TcpConnection::ptr);
    virtual bool onCloseConntection(TcpConnection::ptr);

private:
    // 定时器轮询间隔
    int timerInterval_;

    // 连接超时时间
    int connTimeOut_;

    // 事件循环线程池
    EventLoopThreadPool *threadPool_;

    // 主事件循环，用于接收连接
    EventLoop::ptr mainLoop_;

    // 是否开启https
    bool enable_https;

    // ssl连接上下文
    std::shared_ptr<SSL_CTX> sslCtx_;

    // listen sock
    std::unique_ptr<Socket> servSock_;

    // listen channel
    Channel::ptr listenChannel_;

    MutexType connMtx_;
    ConnectMap connects_;
    
    int evfd_;
};

}

#endif