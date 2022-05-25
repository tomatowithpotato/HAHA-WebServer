#include "./Epoller.h"

namespace haha {

Epoller::Epoller()
    :epfd_(::epoll_create1(EPOLL_CLOEXEC))
    ,events_(kInitEventListSize)
{
}

Epoller::~Epoller() {}

void Epoller::addEvent(int fd, uint32_t event) {
    struct epoll_event ee;
    ::memset(&ee, 0, sizeof(ee));
    ee.data.fd = fd;
    ee.events = event;
    ::epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ee);
}

void Epoller::delEvent(int fd){
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
}

void Epoller::modEvent(int fd, uint32_t event){
    struct epoll_event ee;
    ::memset(&ee, 0, sizeof(ee));
    ee.data.fd = fd;
    ee.events = event;
    ::epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ee);
}

void Epoller::addChannel(Channel* channel){
    /* epoll_event.data是联合体，不能同时对fd和ptr赋值！！！！ */
    struct epoll_event ee;
    ::memset(&ee, 0, sizeof(ee));
    ee.data.ptr = static_cast<void*>(channel);
    // ee.data.fd = fd; 
    ee.events = channel->getEvents();
    ::epoll_ctl(epfd_, EPOLL_CTL_ADD, channel->getFd(), &ee);
}

void Epoller::modChannel(Channel* channel){
    /* epoll_event.data是联合体，不能同时对fd和ptr赋值！！！！ */
    struct epoll_event ee;
    ::memset(&ee, 0, sizeof(ee));
    ee.data.ptr = static_cast<void*>(channel);
    // ee.data.fd = fd;
    ee.events = channel->getEvents();
    ::epoll_ctl(epfd_, EPOLL_CTL_MOD, channel->getFd(), &ee);
}

void Epoller::delChannel(Channel* channel){
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, channel->getFd(), nullptr);
}


int Epoller::wait(int timeoutMs) {
    int num_events = ::epoll_wait(epfd_, events_.data(), static_cast<int>(events_.size()), timeoutMs);
    if (num_events < 0){
        if(errno != EINTR){
            throw "epoll wait error";   
        }
    }

    if (num_events >= (int)events_.size())
        events_.resize(num_events * 2);
    
    return num_events;
}

int Epoller::getEventFd(size_t i){
    return events_[i].data.fd;
}

uint32_t Epoller::getReturnEvents(size_t i){
    return events_[i].events;
}

Channel* Epoller::getActiveChannel(size_t i){
    return static_cast<Channel*>(events_[i].data.ptr);
}

}