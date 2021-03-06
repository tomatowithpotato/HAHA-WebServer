#include "TcpConnection.h"

namespace haha{

TcpConnection::TcpConnection(const Socket &sock)
    :sock_(std::make_shared<Socket>(sock))
    ,recver_(std::make_shared<Buffer>())
    ,sender_(std::make_shared<Buffer>())
    ,fileSender_(nullptr)
    ,keepAlive_(false){
    
}

TcpConnection::TcpConnection(Socket::ptr sock)
    :sock_(sock)
    ,recver_(std::make_shared<Buffer>())
    ,sender_(std::make_shared<Buffer>())
    ,fileSender_(nullptr)
    ,keepAlive_(false){
}


TcpConnection::TcpConnection()
    :sock_(std::make_shared<Socket>())
    ,recver_(std::make_shared<Buffer>())
    ,sender_(std::make_shared<Buffer>())
    ,fileSender_(nullptr)
    ,keepAlive_(false){
}


TcpConnection::status TcpConnection::recv(){
    int len;
    int recvBytes = sock_->recv(recver_, &len);

    if(len > 0){
        /* 阻塞情况下 len > 0 */
        return status(recvBytes, errno, status::AGAIN);
    }
    else if(len < 0 && errno == EAGAIN){
        /* 非阻塞情况下 len < 0 且 error == EAGAIN */
        return status(recvBytes, errno, status::AGAIN);
    }
    else if(len == 0){
        /* len == 0 说明连接关闭 */
        return status(recvBytes, errno, status::CLOSED);
    }
    return status(recvBytes, errno, status::ERROR);
}

TcpConnection::status TcpConnection::send(){
    int len = -10086;
    int sendBytes = 0;

    if(sender_->ReadableBytes() > 0){
        sendBytes = sock_->send(sender_, &len);
    }
    if(fileSender_ && fileSender_->sendable() 
        && sender_->ReadableBytes() == 0 
        && (len > 0 || len == -10086)){
        sendBytes = fileSender_->send(&len);
    }

    if(len > 0 && !sendable()){
        return status(sendBytes, errno, status::COMPLETED);
    }
    else if(len < 0 && errno == EAGAIN && sendable() > 0){
        /* 非阻塞情况下 len < 0 且 error == EAGAIN */
        return status(sendBytes, errno, status::AGAIN);
    }
    else if(len == 0){
        /* len == 0 说明连接关闭 */
        return status(sendBytes, errno, status::CLOSED);
    }
    return status(sendBytes, errno, status::ERROR);
}

void TcpConnection::close(){
    disconnected_ = true;
    channel_ = nullptr;
}

bool TcpConnection::sendable(){
    if(fileSender_){
        return fileSender_->sendable() || sender_->ReadableBytes();
    }
    return sender_->ReadableBytes();
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
    ReadWriteLock::RAIIReadLock rlock(disconnMtx_);
    return disconnected_;
}

void TcpConnection::setDisconnected(bool is){
    ReadWriteLock::RAIIWriteLock wlock(disconnMtx_);
    disconnected_ = is;
}

void TcpConnection::setFileStream(const char *file_path){
    fileSender_ = std::make_shared<FileUtil::FileSendStream>(file_path, sock_->getFd(), sock_->isBlocked());
}

}