#ifndef __CHANNEL__
#define __CHANNEL__
#include <Socket.h>
#include <functional>
#include <memory>

class EventLoop;

class Channel {
public:
  Channel(EventLoop *ep, Socket *sock);
  ~Channel();
  Channel() = delete;
  Channel(const Channel &ch) = delete;
  Channel(Channel &&ch) = delete;
  Channel &operator=(const Channel &ch) = delete;
  Channel &operator=(Channel &&ch) = delete;

  void enableReading();
  void setReadCallBack(std::function<void()> &&callback);
  void enableWriting();
  void setWriteCallBack(std::function<void()> &&callback);
  void enableET();
  // void enableThreadPool();
  int getEvents();
  void setRevents(int ev);
  int getRevents();
  Socket *getSocket();
  bool isInEpoll();
  void setInEpoll(bool flag = true);
  void handleEvent();

  static const uint8_t EVENT_READ = 1;
  static const uint8_t EVENT_WRITE = 2;
  static const uint8_t EVENT_ET = 4;

private:
  EventLoop *mEvlp = nullptr;
  std::unique_ptr<Socket> mSocket;
  int events = 0;
  int revents = 0;
  bool inEpoll = false;
  std::function<void()> mReadCallback;
  std::function<void()> mWriteCallback;
  // bool mUseThreadPool = false;
};

#endif