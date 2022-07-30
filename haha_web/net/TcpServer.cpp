#include "TcpServer.h"
#include <iostream>
#include <signal.h>

namespace haha{

TcpServer::TcpServer()
    :timerInterval_(config::GET_CONFIG<int>("server.timerInterval", 5))
    ,connTimeOut_(config::GET_CONFIG<int>("server.timeout", 120))
    ,threadPool_(&EventLoopThreadPool::getInstance())
    ,mainLoop_(threadPool_->getBaseLoop())
    ,servSock_(Socket::FDTYPE::NONBLOCK)
{
    servSock_.enableReuseAddr(true);
    servSock_.enableReusePort(true);
}


void TcpServer::start(const InetAddress &address){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    servSock_.bind(address);
    servSock_.listen();
    listenChannel_ = std::make_shared<Channel>(mainLoop_.get(), servSock_.getFd(), false);
    listenChannel_->setReadCallback(std::bind(&TcpServer::handleServerAccept, this));
    listenChannel_->setEvents(EPOLLIN | kServerEvent);

    threadPool_->start();

    mainLoop_->addChannel(listenChannel_.get());
    mainLoop_->loop(timerInterval_);
}

void TcpServer::start(){
    InetAddress address(config::GET_CONFIG<int>("server.port", 9999));
    HAHA_LOG_INFO(HAHA_LOG_ASYNC_FILE_ROOT()) << "server port: " << address.getPort();
    start(address);
}


void TcpServer::handleServerAccept(){
    HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "handleServerAccept";
    Socket::ptr sock = servSock_.accept();
    if(sock == nullptr){
        return;
    }

    sock->setNonBlocking();
    sock->ignoreSIGPIPE(true);

    int connfd = sock->getFd();
    
    TcpConnection::ptr conn = std::make_shared<TcpConnection>(sock);

    {
        MutexType::RAIILock lock(connMtx_);
        connects_[connfd] = conn;
    }

    TcpConnection::weak_ptr weak_conn(conn);
    auto loop = threadPool_->getNextLoop();

    auto channel = std::make_shared<Channel>(loop.get(), connfd, false);
    channel->setEvents(EPOLLIN | kConnectionEvent);
    channel->setReadCallback(std::bind(&TcpServer::handleConnectionRead, this, weak_conn));
    channel->setWriteCallback(std::bind(&TcpServer::handleConnectionWrite, this, weak_conn));
    channel->setCloseCallback(std::bind(&TcpServer::handleConnectionClose, this, weak_conn));
    conn->setChannel(channel);

    loop->runInLoop(std::bind(&TcpServer::onConnect, this, weak_conn));
}


void TcpServer::handleConnectionRead(TcpConnection::weak_ptr weak_conn) {
    HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "handleConnectionRead";
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "conn already destroy";
        return;
    }
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;

    auto loop = conn->getChannel()->getEventLoop();
    loop->assertInLoopThread();

    loop->adjustTimer(Timer(
        conn->getFd(),
        TimeStamp::nowSecond(connTimeOut_),
        nullptr
    ));

    loop->runInLoop(std::bind(&TcpServer::onRecv, this, weak_conn));
    // HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
}


void TcpServer::handleConnectionWrite(TcpConnection::weak_ptr weak_conn) {
    HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "handleConnectionWrite";
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "conn already destroy";
        return;
    }
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;

    auto loop = conn->getChannel()->getEventLoop();
    loop->assertInLoopThread();

    loop->adjustTimer(Timer(
        conn->getFd(),
        TimeStamp::nowSecond(connTimeOut_),
        nullptr
    ));

    loop->runInLoop(std::bind(&TcpServer::onSend, this, weak_conn));
}


void TcpServer::handleConnectionClose(TcpConnection::weak_ptr weak_conn) {
    HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "handleConnectionClose";
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "conn already destroy";
        return;
    }
    if (conn->isDisconnected() || conn->getChannel() == nullptr)
        return;

    onCloseConntection(conn);
    
    auto loop = conn->getChannel()->getEventLoop();
    loop->assertInLoopThread();

    loop->delChannel(conn->getChannel().get());
    conn->setDisconnected(true);

    // 从连接表中删除连接，因为可能有多个线程同时删，因此必须加锁保护
    {
        MutexType::RAIILock lock(connMtx_);
        auto it = connects_.find(conn->getFd());
        if(it != connects_.end()){
            connects_.erase(it);
            // HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
        }
    }
    
}


void TcpServer::onConnect(TcpConnection::weak_ptr weak_conn){
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "conn already destroy";
        return;
    }

    auto connfd = conn->getFd();
    auto loop = conn->getChannel()->getEventLoop();
    loop->assertInLoopThread();

    onCreateConnection(conn);

    loop->addChannel(conn->getChannel().get());
    loop->addTimer(Timer(
        connfd,
        TimeStamp::nowSecond(connTimeOut_),
        std::bind(&TcpServer::handleConnectionClose, this, weak_conn)
    ));
}


void TcpServer::onRecv(TcpConnection::weak_ptr weak_conn){
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "conn already destroy";
        return;
    }
    TcpConnection::status status = conn->recv();

    auto loop = conn->getChannel()->getEventLoop();
    loop->assertInLoopThread();

    if(status.type == status.CLOSED || status.type == status.ERROR){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "onRecv: CLOSE or ERROR";
        /* 关闭连接 */
        loop->delTimer(Timer(
            conn->getFd(),
            TimeStamp::nowSecond(0),
            nullptr
        ));
        handleConnectionClose(conn);
        return;
    }
    /* 处理接收的数据 */
    if(onMessage(conn) == MESSAGE_STATUS::AGAIN){
        /* 数据不完整，接着读 */
        conn->setEvents(EPOLLIN | kConnectionEvent);
    }
    else{
        /* 全部处理完就准备写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
    }

    loop->modChannel(conn->getChannel().get());
    // HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
}


void TcpServer::onSend(TcpConnection::weak_ptr weak_conn){
    TcpConnection::ptr conn = weak_conn.lock();
    // 检查连接是否被析构
    if(!conn){
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "conn already destroy";
        return;
    }

    TcpConnection::status status = conn->send();
    EventLoop *loop = conn->getChannel()->getEventLoop();
    loop->assertInLoopThread();

    switch (status.type)
    {
    case status.COMPLETED:
        if(conn->isKeepAlive()){
            /* 准备接受新的连接 */
            conn->setEvents(EPOLLIN | kConnectionEvent);
            loop->modChannel(conn->getChannel().get());
        }
        else{
            HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "onSend: COMPLETED, has write " << status.n;
            /* 关闭连接 */
            loop->delTimer(Timer(
                conn->getFd(),
                TimeStamp::nowSecond(0),
                nullptr
            ));
            handleConnectionClose(conn);
            // HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << (std::string("ptr count: ") + std::to_string(conn.use_count()));
        }
        break;
    case status.AGAIN:
        // HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "onSend: AGAIN ";
        /* 没写完，接着写 */
        conn->setEvents(EPOLLOUT | kConnectionEvent);
        loop->modChannel(conn->getChannel().get());
        break;
    default:
        HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "onSend: CLOSE or ERROR";
        /* 关闭连接 */
        loop->delTimer(Timer(
            conn->getFd(),
            TimeStamp::nowSecond(0),
            nullptr
        ));
        handleConnectionClose(conn);
        break;
    }
}

TcpServer::MESSAGE_STATUS TcpServer::onMessage(TcpConnection::ptr conn){
    HAHA_LOG_INFO(HAHA_LOG_ASYNC_FILE_ROOT()) << "onMessage";
    return MESSAGE_STATUS::OK;
}

bool TcpServer::onCreateConnection(TcpConnection::ptr conn){
    HAHA_LOG_INFO(HAHA_LOG_ASYNC_FILE_ROOT()) << "onNewConntection";
    return true;
}

bool TcpServer::onCloseConntection(TcpConnection::ptr conn){
    HAHA_LOG_INFO(HAHA_LOG_ASYNC_FILE_ROOT()) << "onCloseConntection";
    return true;
}

}