#include "Epoll.h"
#include "Channel.h"
#include "Socket.h"
#include "common.h"
#include <cstring>
#include <unistd.h>

#ifdef OS_LINUX

Epoll::Epoll() {
  epfd = epoll_create1(0);
  errif(epfd == -1, "create epoll error");
  memset(events, 0, sizeof(events));
}

Epoll::~Epoll() { close(); }

void Epoll::close() {
  if (epfd == -1) {
    return;
  }
  ::close(epfd);
  epfd = -1;
}

void Epoll::set(Channel *ch) {
  Event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.ptr = ch;
  if (ch->getEvents() & Channel::EVENT_READ) {
    ev.events |= EPOLLIN | EPOLLPRI;
  }
  if (ch->getEvents() & Channel::EVENT_WRITE) {
    ev.events |= EPOLLOUT;
  }
  if (ch->getEvents() & Channel::EVENT_ET) {
    ev.events |= EPOLLET;
  }
  if (ch->isInEpoll()) {
    errif(epoll_ctl(epfd, EPOLL_CTL_MOD, ch->getSocket()->getFd(), &ev) == -1,
          "set epoll error");
  } else {
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, ch->getSocket()->getFd(), &ev) == -1,
          "add epoll error");
    ch->setInEpoll();
  }
}

void Epoll::remove(Channel *ch) {
  int sockfd = ch->getSocket()->getFd();
  errif(epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, nullptr) == -1,
        "epoll delete error");
  ch->setInEpoll(false);
}

std::vector<Channel *> Epoll::poll(int timeout) {
  int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
  std::vector<Channel *> res;
  for (int i = 0; i < nfds; ++i) {
    Channel *ch = (Channel *)events[i].data.ptr;
    int event = events[i].events, tmp = 0;
    if ((event & EPOLLIN) || (event & EPOLLPRI)) {
      tmp |= Channel::EVENT_READ;
    }
    if (event & EPOLLOUT) {
      tmp |= Channel::EVENT_WRITE;
    }
    if (event & EPOLLET) {
      tmp |= Channel::EVENT_ET;
    }
    ch->setRevents(tmp);
    res.push_back(ch);
  }
  return res;
}

#endif

#ifdef OS_MACOS

Epoll::Epoll() {
  epfd = kqueue();
  errif(epfd == -1, "kqueue create error");
  events = new struct kevent[MAX_EVENTS];
  memset(events, 0, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() { close(); }

void Epoll::close() {
  if (epfd == -1) {
    return;
  }
  ::close(epfd);
  epfd = -1;
  delete events[];
}

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> active_channels;
  struct timespec ts;
  memset(&ts, 0, sizeof(ts));
  if (timeout != -1) {
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000 * 1000;
  }
  int nfds = 0;
  if (timeout == -1) {
    nfds = kevent(epfd, NULL, 0, events, MAX_EVENTS, NULL);
  } else {
    nfds = kevent(epfd, NULL, 0, events, MAX_EVENTS, &ts);
  }
  for (int i = 0; i < nfds; ++i) {
    Channel *ch = (Channel *)events[i].udata;
    int event = events[i].filter, tmp = 0;
    if (event == EVFILT_READ) {
      tmp |= (Channel::EVENT_READ | Channel::EVENT_ET);
    }
    if (event == EVFILT_WRITE) {
      tmp |= (Channel::EVENT_WRITE | Channel::EVENT_ET);
    }
    ch->setRevents(tmp);
    active_channels.push_back(ch);
  }
  return active_channels;
}

void Epoll::set(Channel *ch) {
  struct kevent ev[2];
  memset(ev, 0, sizeof(*ev) * 2);
  int n = 0;
  int fd = ch->getSocket()->getFd();
  int op = EV_ADD;
  if (ch->getEvents() & ch->EVENT_ET) {
    op |= EV_CLEAR;
  }
  if (ch->getEvents() & ch->EVENT_READ) {
    EV_SET(&ev[n++], fd, EVFILT_READ, op, 0, 0, ch);
  }
  if (ch->getEvents() & ch->EVENT_WRITE) {
    EV_SET(&ev[n++], fd, EVFILT_WRITE, op, 0, 0, ch);
  }
  int r = kevent(epfd, ev, n, NULL, 0, NULL);
  errif(r == -1, "kqueue add event error");
}

void Epoll::remove(Channel *ch) {
  struct kevent ev[2];
  int n = 0;
  int fd = ch->getSocket()->getFd();
  if (ch->getEvents() & ch->EVENT_READ) {
    EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, ch);
  }
  if (ch->getEvents() & ch->EVENT_WRITE) {
    EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, ch);
  }
  int r = kevent(epfd, ev, n, NULL, 0, NULL);
  errif(r == -1, "kqueue delete event error");
}
#endif