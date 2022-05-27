#ifndef __HAHA_CHANNEL_H__
#define __HAHA_CHANNEL_H__

#include <functional>
#include <sys/epoll.h>
#include <memory>
#include <unistd.h>
#include "base/Buffer.h"
#include "base/noncopyable.h"

#include <iostream>

namespace haha{

class EventLoop;

class Channel : noncopyable {

using ReadCallback = std::function<void()>;
using WriteCallback = std::function<void()>;
using CloseCallback = std::function<void()>;
using ErrorCallback = std::function<void()>;

public:
    typedef std::shared_ptr<Channel> ptr;

    Channel(EventLoop* loop, int fd, bool BlockFd = true);
    ~Channel() {
        std::cout << "~Channel" << std::endl;
        ::close(fd_);
    }

    void handleEvents(uint32_t revents);
    int getFd() const {return fd_;}
    uint32_t getEvents() const {return events_;}
    void setEvents(uint32_t events) {events_ = events;}
    EventLoop* getEventLoop() {return eventloop_;}
    bool isBlockFd() {return isBlockFd_;}

    void setReadCallback(ReadCallback readcb) {readCb_ = readcb;}
    void setWriteCallback(WriteCallback writecb) {writeCb_ = writecb;}
    void setCloseCallback(CloseCallback closecb) {closeCb_ = closecb;}

private:
    int fd_;
    uint32_t events_;
    bool isBlockFd_;

    EventLoop *eventloop_;

    ReadCallback readCb_;
    WriteCallback writeCb_;
    CloseCallback closeCb_;
    ErrorCallback errorCb_;
};

}

#endif