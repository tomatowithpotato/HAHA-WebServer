#include "TcpConnection.h"

namespace haha{

TcpConnection::status TcpConnection::read(){
    ssize_t len = -1;
    int saveErrno;
    do {
        len = recver_.ReadFd(channel_->getFd(), &saveErrno);
        if (len <= 0) {
            break;
        }
        receivedBytes_ += len;
    } while (!channel_->isBlockFd());

    if(len > 0){
        /* 阻塞情况下 len > 0 */
        return status(len, saveErrno, status::COMPLETED);
    }
    else if(len < 0 && saveErrno == EAGAIN){
        /* 非阻塞情况下 len < 0 且 error == EAGAIN */
        return status(len, saveErrno, status::AGAIN);
    }
    else if(len == 0){
        /* len == 0 说明连接关闭 */
        return status(len, saveErrno, status::CLOSED);
    }
    return status(len, saveErrno, status::ERROR);
}

TcpConnection::status TcpConnection::write(){
    ssize_t len = -1;
    int saveErrno;
    do {
        len = sender_.WriteFd(channel_->getFd(), &saveErrno);
        if (len <= 0) {
            break;
        }
        sendBytes_ += len;
        remainBytes_ -= len;
    } while (!channel_->isBlockFd());

    if(len > 0){
        /* 阻塞情况下 len > 0 */
        return status(len, saveErrno, status::COMPLETED);
    }
    else if(len < 0 && saveErrno == EAGAIN){
        /* 非阻塞情况下 len < 0 且 error == EAGAIN */
        return status(len, saveErrno, status::AGAIN);
    }
    else if(len == 0){
        /* len == 0 说明连接关闭 */
        return status(len, saveErrno, status::CLOSED);
    }
    return status(len, saveErrno, status::ERROR);
}

void TcpConnection::close(){
    disconnected_ = true;
    channel_ = nullptr;
}

void TcpConnection::retriveAll(){
    retriveRead();
    retriveWrite();
}

void TcpConnection::retriveRead(){
    recver_.RetrieveAll();
    receivedBytes_ = 0;
}

void TcpConnection::retriveWrite(){
    sender_.RetrieveAll();
    sendBytes_ = 0;
    remainBytes_ = 0;
}

}