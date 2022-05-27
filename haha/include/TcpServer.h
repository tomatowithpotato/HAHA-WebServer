#ifndef __HAHA_TCPSERVER_H__
#define __HAHA_TCPSERVER_H__

#include <memory>
#include <unordered_map>
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
    void handleConnected(int, const InetAddress&);
    void handleConnectionRead(TcpConnection *);
    void handleConnectionWrite(TcpConnection *);
    void handleConnectionClose(TcpConnection *);

    void onWrite(TcpConnection *);
    void onRead(TcpConnection *);

    virtual bool onMessage(TcpConnection *);
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