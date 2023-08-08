#ifndef __EPOLL__
#define __EPOLL__

#include <vector>

#ifdef OS_LINUX
#include <sys/epoll.h>
using Event = epoll_event;
#endif

#ifdef OS_MACOS
#include <sys/event.h>
using Event = struct kevent;
#endif

class Channel;

class Epoll {
public:
  Epoll();
  ~Epoll();
  Epoll(const Epoll &ep) = delete;
  Epoll(Epoll &&ep) = delete;
  Epoll &operator=(const Epoll &ep) = delete;
  Epoll &operator=(Epoll &&ep) = delete;

  void set(Channel *ch);
  void remove(Channel *ch);
  std::vector<Channel *> poll(int timeout = -1);
  void close();

private:
  int epfd = -1;
  static const int MAX_EVENTS = 1024;
#ifdef OS_LINUX
  Event events[MAX_EVENTS];
#endif

#ifdef OS_MACOS
  Event *events = nullptr;
#endif
};

#endif