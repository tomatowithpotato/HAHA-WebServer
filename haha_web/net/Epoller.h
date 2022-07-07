#ifndef __HAHA_EPOLLER_H__
#define __HAHA_EPOLLER_H__

#include <atomic>
#include <functional>
#include <string.h>
#include <sys/epoll.h>
#include <assert.h>
// #include <sys/eventfd.h>
#include <unistd.h>
#include <vector>
#include <memory>

#include "base/noncopyable.h"
#include "Channel.h"
 
namespace haha {

class Channel;

static constexpr auto kServerEvent = EPOLLRDHUP;
static constexpr auto kConnectionEvent = EPOLLET | EPOLLONESHOT | EPOLLRDHUP;

class Epoller : noncopyable {

typedef std::vector<epoll_event> EventList;
typedef std::vector<Channel*> ChannelList;

public:
    enum TrigerMod {LT, ET};
    
    Epoller();
    ~Epoller();

    void addEvent(int fd, uint32_t event);
    void modEvent(int fd, uint32_t event);
    void delEvent(int fd);
    void removeAndCloseEvent(int fd);

    void addChannel(Channel* channel);
    void modChannel(Channel* channel);
    void delChannel(Channel* channel);

    void setTrigerMod(TrigerMod mod) {trigerMod_ = mod;}
    TrigerMod getTrigerMod() {return trigerMod_;}

    Channel* getActiveChannel(size_t i);

    int wait(int timeoutMs = -1);
    int getEventFd(size_t i);
    uint32_t getReturnEvents(size_t i);

private:
    static const int kInitEventListSize = 16;
    int epfd_;
    EventList events_;
    ChannelList activeChannels_;
    TrigerMod trigerMod_;
};

}

#endif