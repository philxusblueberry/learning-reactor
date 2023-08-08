#ifndef __ACCEPTOR__
#define __ACCEPTOR__

#include <functional>

class EventLoop;
class Socket;
class Channel;

class Acceptor {
public:
  Acceptor(EventLoop *evlp);
  ~Acceptor();
  Acceptor() = delete;
  Acceptor(const Acceptor &at) = delete;
  Acceptor(Acceptor &&at) = delete;
  Acceptor &operator=(const Acceptor &at) = delete;
  Acceptor &operator=(Acceptor &&at) = delete;

  void acception();
  int close();
  int setNewConnectionCallback(std::function<void(Socket *)> &&func);

private:
  EventLoop *mEp = nullptr;
  Channel *mAcceptChannel = nullptr;
  std::function<void(Socket *)> mCallback;
};

#endif