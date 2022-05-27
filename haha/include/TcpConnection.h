#ifndef __HAHA_TCPCONNECTION_H
#define __HAHA_TCPCONNECTION_H

#include <memory>
#include <utility>
#include "InetAddress.h"
#include "Channel.h"

#include <iostream>

namespace haha{

class TcpConnection{
public:

    static const int TimeOut = 15;

    struct status{
        ssize_t n;
        int errorNo;
        enum TYPE{
            AGAIN,
            COMPLETED,
            ERROR,
            CLOSED,
        } type;
        status() = default;
        status(ssize_t n, int errorNo, TYPE type){
            this->n = n;
            this->errorNo = errorNo;
            this->type = type;
        }  
    };

    TcpConnection(const InetAddress &inetAddress):inetAddress_(inetAddress){}
    TcpConnection() = default;

    ~TcpConnection() {
        std::cout << "~TcpConnection" << std::endl;
    }

    status read();
    status write();
    void close();

    void init(const InetAddress &inetAddress) {
        inetAddress_ = inetAddress;
    }

    void setChannel(Channel::ptr channel) { channel_ = channel; }

    std::string getIp() const noexcept { return inetAddress_.getIp(); }
    uint16_t getPort() const noexcept { return inetAddress_.getPort(); }
    sockaddr *getSockAddr() const noexcept { return inetAddress_.getSockAddr(); }
    int getFd() { return channel_->getFd(); }
    Channel::ptr getChannel() {return channel_;}
    void setEvents(uint32_t events) { channel_->setEvents(events); }

    bool isKeepAlive() const { return keep_alive_;}
    void setKeepAlive(bool is) { keep_alive_ = is; }

    bool isDisconnected() const { return disconnected_; }
    void setDisconnected(bool is) { disconnected_ = is; }

    Buffer& getRecver() { return recver_; }
    Buffer& getSender() { return sender_; }

    bool readable() { return recver_.ReadableBytes() > 0; }

    void retriveRead();
    void retriveWrite();
    void retriveAll();

private:
    Buffer recver_;
    Buffer sender_;
    Channel::ptr channel_;
    InetAddress inetAddress_;
    bool disconnected_;

    bool keep_alive_;

    bool receiveComplete_ = false;

    /* 已接收的字节数 */
    int receivedBytes_ = 0;

    /* 已发送的字节数 */
    int sendBytes_ = 0;
    /* 未发送的字节数 */
    int remainBytes_ = 0;
};

}

#endif