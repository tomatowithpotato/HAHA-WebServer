#include "TcpServer.h"
#include <iostream>

namespace haha{

TcpServer::TcpServer()
    :servSock_(Socket::FDTYPE::NONBLOCK)
    ,threadPool_(&ThreadPool::getInstance())
    ,timeoutInterval_(5)
{
    servSock_.enableReuseAddr(true);
    servSock_.enableReusePort(true);
}

void TcpServer::start(const InetAddress &address){
    servSock_.bind(address);
    servSock_.listen();
    listenChannel_ = std::make_shared<Channel>(&eventLoop_, servSock_.getFd(), false);
    listenChannel_->setReadCallback(std::bind(&TcpServer::handleServerAccept, this));
    listenChannel_->setEvents(EPOLLIN | kServerEvent);

    threadPool_->start();

    eventLoop_.addChannel(listenChannel_.get());
    eventLoop_.loop(timeoutInterval_);
}

void TcpServer::handleServerAccept(){
    const auto p = servSock_.accept();
    int connfd = p.first;
    if (connfd <= 0){
        return;
    }
    handleConnected(connfd, p.second);
}

void TcpServer::handleConnected(int connfd, const InetAddress &inetAdress){
    sockops::setNonBlocking(connfd);
    connects_[connfd] = std::make_shared<TcpConnection>(inetAdress);
    auto connection = connects_[connfd];
    connection->setChannel(std::make_shared<Channel>(&eventLoop_, connfd, false));
    connection->setEvents(EPOLLIN | kConnectionEvent);
    connection->getChannel()->setReadCallback(std::bind(&TcpServer::handleConnectionRead, this, connection.get()));
    connection->getChannel()->setWriteCallback(std::bind(&TcpServer::handleConnectionWrite, this, connection.get()));
    connection->getChannel()->setCloseCallback(std::bind(&TcpServer::handleConnectionClose, this, connection.get()));

    onNewConntection(connection.get());
    
    eventLoop_.addChannel(connection->getChannel().get());
    eventLoop_.addTimer(Timer(
        connfd,
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        std::bind(&TcpServer::handleConnectionClose, this, connection.get())
    ));
}

void TcpServer::handleConnectionRead(TcpConnection *conn) {
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;

    threadPool_->addTask(std::bind(&TcpServer::onRead, this, conn));
}

void TcpServer::handleConnectionWrite(TcpConnection *conn) {
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;

    threadPool_->addTask(std::bind(&TcpServer::onWrite, this, conn));
}

void TcpServer::handleConnectionClose(TcpConnection *conn) {
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;
    
    onCloseConntection(conn);

    eventLoop_.delChannel(conn->getChannel().get());
    connects_.erase(conn->getFd());
}

void TcpServer::onRead(TcpConnection *conn){
    TcpConnection::status status = conn->read();
    if(status.type == status.CLOSED || status.type == status.ERROR){
        handleConnectionClose(conn);
        return;
    }
    /* 处理接收的数据 */
    if(onMessage(conn)){
        /* 处理完就准备写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
    }
    else{
        /* 数据不完整，接着读 */
        conn->setEvents(EPOLLIN | kConnectionEvent);
    }
    eventLoop_.modChannel(conn->getChannel().get());
}

void TcpServer::onWrite(TcpConnection *conn){
    TcpConnection::status status = conn->write();
    if(status.type == status.CLOSED){
        if(conn->isKeepAlive()){
            if(conn->getSender().ReadableBytes()){
                /* 还有数据，接着写 */
                conn->setEvents(EPOLLOUT | kConnectionEvent);
            }
            else{
                /* 转为读 */
                conn->setEvents(EPOLLIN | kConnectionEvent);
            }
            eventLoop_.modChannel(conn->getChannel().get());
        }
    }
    else if(status.type == status.AGAIN){
        /* 没写完，接着写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
        eventLoop_.modChannel(conn->getChannel().get());
    }
    else{
        handleConnectionClose(conn);
    }
}

bool TcpServer::onMessage(TcpConnection *conn){
    std::cout << "onMessage" << std::endl;
    return true;
}

bool TcpServer::onNewConntection(TcpConnection *conn){
    std::cout << "onNewConntection" << std::endl;
    return true;
}

bool TcpServer::onCloseConntection(TcpConnection *conn){
    std::cout << "onCloseConntection" << std::endl;
    return true;
}

}