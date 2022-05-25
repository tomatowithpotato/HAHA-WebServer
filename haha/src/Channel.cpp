#include "Channel.h"

namespace haha{

Channel::Channel(EventLoop* loop, int fd, bool BlockFd)
  :eventloop_(loop)
  ,fd_(fd)
  ,isBlockFd_(BlockFd){

}

void Channel::handleEvents(uint32_t revents){
    if (revents & (EPOLLERR | EPOLLRDHUP | EPOLLHUP)) {
      if (closeCb_)
        closeCb_();
    } else if (revents & EPOLLIN) {
      if (readCb_)
        readCb_();
    } else if (revents & EPOLLOUT) {
      if (writeCb_)
        writeCb_();
    }
}

}