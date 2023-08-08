#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

EventLoop::EventLoop() { mEp = std::make_unique<Epoll>(); }

EventLoop::~EventLoop() {}

void EventLoop::loop() {
  while (!quit) {
    std::vector<Channel *> res = mEp->poll();
    for (auto &&ch : res) {
      ch->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *ch) { mEp->set(ch); }

Epoll *EventLoop::getEpoll() { return mEp.get(); }