#ifndef __HAHA_TCPCONNECTION_H
#define __HAHA_TCPCONNECTION_H

#include <memory>
#include <utility>
#include <iostream>

#include "base/Mutex.h"
#include "base/ReadWriteLock.h"
#include "base/FileUtil.h"
#include "log/Log.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

namespace haha{

class TcpConnection{
public:
    typedef std::shared_ptr<TcpConnection> ptr;
    typedef std::weak_ptr<TcpConnection> weak_ptr;

    struct status{
        int n;
        int errorNo;
        enum TYPE{
            AGAIN,
            COMPLETED,
            ERROR,
            CLOSED,
        } type;
        status() = default;
        status(int n, int errorNo, TYPE type){
            this->n = n;
            this->errorNo = errorNo;
            this->type = type;
        }  
    };

    TcpConnection(const Socket &sock);
    TcpConnection(Socket::ptr sock);
    TcpConnection();

    ~TcpConnection() {
        // HAHA_LOG_DEBUG(HAHA_LOG_ASYNC_FILE_ROOT()) << "~TcpConnection";
    }

    status recv();
    status send();
    void close();

    void init(const Socket& sock) {
        sock_ = std::make_shared<Socket>(sock);
    }

    void setChannel(Channel::ptr channel) { channel_ = channel; }
    void setRecvBuffer(Buffer::ptr buffer) { recver_ = buffer; }
    void setSendBuffer(Buffer::ptr buffer) { sender_ = buffer; }

    std::string getLocalIp() const noexcept { return sock_->getLocalAddress().getIp(); }
    uint16_t getLocalPort() const noexcept { return sock_->getLocalAddress().getPort(); }
    sockaddr *getLocalSockAddr() const noexcept { return sock_->getLocalAddress().getSockAddr(); }

    std::string geRemoteIp() const noexcept { return sock_->getRemoteAddress().getIp(); }
    uint16_t getRemotePort() const noexcept { return sock_->getRemoteAddress().getPort(); }
    sockaddr *getRemoteSockAddr() const noexcept { return sock_->getRemoteAddress().getSockAddr(); }

    int getFd() { return sock_->getFd(); }
    Channel::ptr getChannel() {return channel_;}
    void setEvents(uint32_t events) { channel_->setEvents(events); }

    bool isKeepAlive() const { return keepAlive_;}
    void setKeepAlive(bool is) { keepAlive_ = is; }

    bool isDisconnected() const;
    void setDisconnected(bool is);

    Buffer::ptr getRecver() { return recver_; }
    Buffer::ptr getSender() { return sender_; }

    /* ??????????????????????????? */
    bool sendable();

    // ?????????????????????
    void retriveRecver();
    // ?????????????????????
    void retriveSender();
    void retriveAll();

    std::shared_ptr<void> getContext() { return context_; }
    void setContext(std::shared_ptr<void> context) { context_ = context; }
    void clearContext() { context_ = nullptr; }

    void setFileStream(const char *file_path);

private:
    Socket::ptr sock_;          // socket
    Buffer::ptr recver_;        // ????????????
    Buffer::ptr sender_;        // ????????????
    Channel::ptr channel_;      // ???????????????

    FileUtil::FileSendStream::ptr fileSender_; // ???????????????

    bool disconnected_ = false;
    bool keepAlive_ = false;

    std::shared_ptr<void> context_;     // ?????????????????????????????????http??????????????????????????????

    mutable ReadWriteLock disconnMtx_;  // ????????????????????????


    // /* ????????????????????? */
    // int receivedBytes_ = 0;

    // /* ????????????????????? */
    // int sendBytes_ = 0;
    // /* ????????????????????? */
    // int remainBytes_ = 0;
};

}

#endif