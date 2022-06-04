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
    Socket::ptr sock = servSock_.accept();
    if(sock == nullptr){
        return;
    }
    handleConnected(sock);
}

void TcpServer::handleConnected(Socket::ptr sock){
    sock->setNonBlocking();
    int connfd = sock->getFd();
    connects_[connfd] = std::make_shared<TcpConnection>(sock);
    auto connection = connects_[connfd];
    connection->setChannel(std::make_shared<Channel>(&eventLoop_, connfd, false));
    connection->setEvents(EPOLLIN | kConnectionEvent);
    connection->getChannel()->setReadCallback(std::bind(&TcpServer::handleConnectionRead, this, connection.get()));
    connection->getChannel()->setWriteCallback(std::bind(&TcpServer::handleConnectionWrite, this, connection.get()));
    connection->getChannel()->setCloseCallback(std::bind(&TcpServer::handleConnectionClose, this, connection.get()));

    onNewConntection(connection.get());
    
    // 单loop模式下，就是eventLoop_
    // 未来要扩展为one loop per thread，预留
    auto loop = connection->getChannel()->getEventLoop();

    loop->addChannel(connection->getChannel().get());
    loop->addTimer(Timer(
        connfd,
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        std::bind(&TcpServer::handleConnectionClose, this, connection.get())
    ));
}

void TcpServer::handleConnectionRead(TcpConnection *conn) {
    HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "handleConnectionRead";
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;
    auto loop = conn->getChannel()->getEventLoop();
    loop->adjustTimer(Timer(
        conn->getFd(),
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        std::bind(&TcpServer::handleConnectionClose, this, conn)
    ));
    threadPool_->addTask(std::bind(&TcpServer::onRecv, this, conn));
}

void TcpServer::handleConnectionWrite(TcpConnection *conn) {
    HAHA_LOG_DEBUG(HAHA_LOG_ROOT()) << "handleConnectionWrite";
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;
    auto loop = conn->getChannel()->getEventLoop();
    loop->adjustTimer(Timer(
        conn->getFd(),
        TimeStamp::nowSecond(TcpConnection::TimeOut),
        std::bind(&TcpServer::handleConnectionClose, this, conn)
    ));
    threadPool_->addTask(std::bind(&TcpServer::onSend, this, conn));
}

void TcpServer::handleConnectionClose(TcpConnection *conn) {
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;
    
    onCloseConntection(conn);
    
    auto loop = conn->getChannel()->getEventLoop();
    loop->delChannel(conn->getChannel().get());
    connects_.erase(conn->getFd());
}

void TcpServer::onRecv(TcpConnection *conn){
    TcpConnection::status status = conn->read();
    if(status.type == status.CLOSED || status.type == status.ERROR){
        handleConnectionClose(conn);
        return;
    }
    /* 处理接收的数据 */
    if(onMessage(conn) == MESSAGE_STATUS::OK){
        /* 处理完就准备写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
    }
    else if(onMessage(conn) == MESSAGE_STATUS::AGAIN){
        /* 数据不完整，接着读 */
        conn->setEvents(EPOLLIN | kConnectionEvent);
    }
    else{
        /* 出现问题，关闭 */
        handleConnectionClose(conn);
    }

    auto loop = conn->getChannel()->getEventLoop();
    loop->modChannel(conn->getChannel().get());
}

void TcpServer::onSend(TcpConnection *conn){
    TcpConnection::status status = conn->write();
    if(status.type == status.CLOSED){
        if(conn->isKeepAlive()){
            if(conn->sendable()){
                /* 还有数据，接着写 */
                conn->setEvents(EPOLLOUT | kConnectionEvent);
            }
            else{
                conn->retriveSender();
                /* 转为读 */
                conn->setEvents(EPOLLIN | kConnectionEvent);
            }
            auto loop = conn->getChannel()->getEventLoop();
            loop->modChannel(conn->getChannel().get());
        }
        else{
            handleConnectionClose(conn);
        }
    }
    else if(status.type == status.AGAIN){
        /* 没写完，接着写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
        auto loop = conn->getChannel()->getEventLoop();
        loop->modChannel(conn->getChannel().get());
    }
    else{
        handleConnectionClose(conn);
    }
}

TcpServer::MESSAGE_STATUS TcpServer::onMessage(TcpConnection *conn){
    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "onMessage";
    return MESSAGE_STATUS::OK;
}

bool TcpServer::onNewConntection(TcpConnection *conn){
    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "onNewConntection";
    conn->getSender()->Append("hahaha");
    return true;
}

bool TcpServer::onCloseConntection(TcpConnection *conn){
    HAHA_LOG_INFO(HAHA_LOG_ROOT()) << "onCloseConntection";
    return true;
}

}