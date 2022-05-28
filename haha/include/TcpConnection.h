#ifndef __HAHA_TCPCONNECTION_H
#define __HAHA_TCPCONNECTION_H

#include <memory>
#include <utility>
#include <iostream>

#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

namespace haha{

class TcpConnection{
public:

    static const int TimeOut = 15;

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
        std::cout << "~TcpConnection" << std::endl;
    }

    status read();
    status write();
    void close();

    void init(const Socket& sock) {
        sock_ = std::make_shared<Socket>(sock);
    }

    void setChannel(Channel::ptr channel) { channel_ = channel; }

    std::string getLocalIp() const noexcept { return sock_->getLocalAddress().getIp(); }
    uint16_t getLocalPort() const noexcept { return sock_->getLocalAddress().getPort(); }
    sockaddr *getLocalSockAddr() const noexcept { return sock_->getLocalAddress().getSockAddr(); }

    std::string geRemoteIp() const noexcept { return sock_->getRemoteAddress().getIp(); }
    uint16_t getRemotePort() const noexcept { return sock_->getRemoteAddress().getPort(); }
    sockaddr *getRemoteSockAddr() const noexcept { return sock_->getRemoteAddress().getSockAddr(); }

    int getFd() { return channel_->getFd(); }
    Channel::ptr getChannel() {return channel_;}
    void setEvents(uint32_t events) { channel_->setEvents(events); }

    bool isKeepAlive() const { return keep_alive_;}
    void setKeepAlive(bool is) { keep_alive_ = is; }

    bool isDisconnected() const { return disconnected_; }
    void setDisconnected(bool is) { disconnected_ = is; }

    Buffer::ptr getRecver() { return recver_; }
    Buffer::ptr getSender() { return sender_; }

    /* 是否还有数据没发送 */
    bool sendable() { return sender_->ReadableBytes() > 0; }

    void retriveRead();
    void retriveWrite();
    void retriveAll();

private:
    Socket::ptr sock_;          // socket
    Buffer::ptr recver_;        // 接收缓存
    Buffer::ptr sender_;        // 发送缓存
    Channel::ptr channel_;      // 事件回调器
    bool disconnected_;

    bool keep_alive_;

    bool receiveComplete_ = false;

    // /* 已接收的字节数 */
    // int receivedBytes_ = 0;

    // /* 已发送的字节数 */
    // int sendBytes_ = 0;
    // /* 未发送的字节数 */
    // int remainBytes_ = 0;
};

}

#endif