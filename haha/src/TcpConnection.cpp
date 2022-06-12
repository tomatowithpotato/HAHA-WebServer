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


TcpConnection::status TcpConnection::recv(){
    int len;
    int recvBytes = sock_->recv(recver_, &len);

    if(len > 0){
        /* 阻塞情况下 len > 0 */
        return status(len, errno, status::AGAIN);
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

TcpConnection::status TcpConnection::send(){
    int len = -10086;
    int sendBytes;

    if(sender_->ReadableBytes() > 0){
        sendBytes = sock_->send(sender_, &len);
    }
    if(fileSender_ && fileSender_->sendable() 
        && sender_->ReadableBytes() == 0 && (len > 0 || len == -10086)){
        sendBytes = fileSender_->send(&len);
    }

    if(len > 0){
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

bool TcpConnection::sendable(){
    if(fileSender_){
        return fileSender_->sendable() || sender_->ReadableBytes() > 0;
    }
    return sender_->ReadableBytes() > 0;
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

void TcpConnection::setFileStream(const char *file_path){
    fileSender_ = std::make_shared<FileUtil::FileSender>(file_path, sock_->getFd(), sock_->isBlocked());
}

}