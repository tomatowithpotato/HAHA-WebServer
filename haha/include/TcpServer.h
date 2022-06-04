#ifndef __HAHA_TCPSERVER_H__
#define __HAHA_TCPSERVER_H__

#include <memory>
#include <unordered_map>
#include "base/Log.h"
#include "InetAddress.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"

namespace haha{

class TcpServer{

public:
    typedef std::unordered_map<int, std::shared_ptr<TcpConnection>> ConnectMap;

    enum MESSAGE_STATUS{
        OK,
        AGAIN,
        CLOSE,
    };

    TcpServer();
    void start(const InetAddress &address);
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
    void handleConnected(Socket::ptr);
    void handleConnectionRead(TcpConnection *);
    void handleConnectionWrite(TcpConnection *);
    void handleConnectionClose(TcpConnection *);

    // virtual bool keepSend();
    // virtual bool keepRecv();

    void onSend(TcpConnection *);
    void onRecv(TcpConnection *);

    virtual MESSAGE_STATUS onMessage(TcpConnection *);
    virtual bool onNewConntection(TcpConnection *);
    virtual bool onCloseConntection(TcpConnection *);
private:
    int timeoutInterval_;
    EventLoop eventLoop_;
    ThreadPool *threadPool_;
    Socket servSock_;

    Channel::ptr listenChannel_;
    ConnectMap connects_;
    
    int evfd_;
};

}

#endif