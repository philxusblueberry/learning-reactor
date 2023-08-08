#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
// #include "ThreadPool.h"

Channel::Channel(EventLoop *ep, Socket *sock) : mEvlp(ep) {
  mSocket = std::unique_ptr<Socket>(sock);
}

Channel::~Channel() {
  events = 0;
  revents = 0;
  inEpoll = false;
}

int Channel::getEvents() { return events; }

void Channel::enableReading() {
  events |= EVENT_READ;
  mEvlp->updateChannel(this);
}

void Channel::enableWriting() {
  events |= EVENT_WRITE;
  mEvlp->updateChannel(this);
}

void Channel::enableET() {
  events |= EVENT_ET;
  mEvlp->updateChannel(this);
}

// void Channel::enableThreadPool() {
//     mUseThreadPool = true;
// }

void Channel::setRevents(int ev) { revents = ev; }

int Channel::getRevents() { return revents; }

Socket *Channel::getSocket() { return mSocket.get(); }

bool Channel::isInEpoll() { return inEpoll; }

void Channel::setInEpoll(bool flag) { inEpoll = flag; }

void Channel::handleEvent() {
  if (revents & (EVENT_READ)) {
    // printf("xxl check %s %d\n", __FUNCTION__, __LINE__);
    if (mReadCallback) {
      // if (mUseThreadPool) {
      //     mEvlp->getThreadPool()->add(mReadCallback);
      // } else {
      // printf("xxl check %s %d\n", __FUNCTION__, __LINE__);
      mReadCallback();
      // }
    }
  }
  if (revents & EVENT_WRITE) {
    // printf("xxl check %s %d\n", __FUNCTION__, __LINE__);
    if (mWriteCallback) {
      // if (mUseThreadPool) {
      //     mEvlp->getThreadPool()->add(mWriteCallback);
      // } else {
      mWriteCallback();
      // }
    }
  }
}

void Channel::setReadCallBack(std::function<void()> &&callback) {
  mReadCallback = callback;
}

void Channel::setWriteCallBack(std::function<void()> &&callback) {
  mWriteCallback = callback;
}
