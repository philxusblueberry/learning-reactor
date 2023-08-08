#ifndef __EVENT_LOOP__
#define __EVENT_LOOP__

#include <memory>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
public:
  EventLoop();
  ~EventLoop();
  EventLoop(const EventLoop &ev) = delete;
  EventLoop(EventLoop &&ev) = delete;
  EventLoop &operator=(const EventLoop &ev) = delete;
  EventLoop &operator=(EventLoop &&ev) = delete;

  void loop();
  void updateChannel(Channel *ch);
  Epoll *getEpoll();

private:
  std::unique_ptr<Epoll> mEp;
  bool quit = false;
};

#endif