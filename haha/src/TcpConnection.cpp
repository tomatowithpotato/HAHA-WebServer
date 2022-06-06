#include "TcpConnection.h"

namespace haha{

TcpConnection::TcpConnection(const Socket &sock)
    :sock_(std::make_shared<Socket>(sock))
    ,recver_(std::make_shared<Buffer>())
    ,sender_(std::make_shared<Buffer>()){
    
}

TcpConnection::TcpConnection(Socket::ptr sock)
    :sock_(sock)
    ,recver_(std::make_shared<Buffer>())
    ,sender_(std::make_shared<Buffer>()){
}


TcpConnection::TcpConnection()
    :sock_(std::make_shared<Socket>())
    ,recver_(std::make_shared<Buffer>())
    ,sender_(std::make_shared<Buffer>()){
}


TcpConnection::status TcpConnection::read(){
    int len = sock_->recv(recver_);

    if(len > 0){
        /* 阻塞情况下 len > 0 */
        return status(len, errno, status::COMPLETED);
    }
    else if(len < 0 && errno == EAGAIN){
        /* 非阻塞情况下 len < 0 且 error == EAGAIN */
        return status(len, errno, status::AGAIN);
    }
    else if(len == 0){
        /* len == 0 说明连接关闭 */
        return status(len, errno, status::CLOSED);
    }
    return status(len, errno, status::ERROR);
}

TcpConnection::status TcpConnection::write(){
    int len = sock_->send(sender_);

    if(len > 0){
        /* 阻塞情况下 len > 0 */
        return status(len, errno, status::COMPLETED);
    }
    else if(len < 0 && errno == EAGAIN){
        /* 非阻塞情况下 len < 0 且 error == EAGAIN */
        return status(len, errno, status::AGAIN);
    }
    else if(len == 0){
        /* len == 0 说明连接关闭 */
        return status(len, errno, status::CLOSED);
    }
    return status(len, errno, status::ERROR);
}

void TcpConnection::close(){
    disconnected_ = true;
    channel_ = nullptr;
}

void TcpConnection::retriveAll(){
    retriveRecver();
    retriveSender();
}

void TcpConnection::retriveRecver(){
    recver_->RetrieveAll();
}

void TcpConnection::retriveSender(){
    sender_->RetrieveAll();
}

bool TcpConnection::isDisconnected() const {
    ReadWriteLock::RallReadLock rlock(disconnMtx_);
    return disconnected_;
}

void TcpConnection::setDisconnected(bool is){
    ReadWriteLock::RallWriteLock wlock(disconnMtx_);
    disconnected_ = is;
}

}