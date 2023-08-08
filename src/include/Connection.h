#ifndef __CONNECTION__
#define __CONNECTION__

#include <Buffer.h>
#include <functional>
#include <memory>

class Channel;
class EventLoop;
class Socket;

class Connection {
public:
  Connection(EventLoop *evlp, Socket *soc);
  ~Connection();
  Connection() = delete;
  Connection(const Connection &con) = delete;
  Connection(Connection &&con) = delete;
  Connection &operator=(const Connection &con) = delete;
  Connection &operator=(Connection &&con) = delete;

  void handleEvent();
  void setCloseCallback(std::function<void(int)> &&callback);
  void setReadCallback(std::function<void(Connection *)> &&callback);
  void setReadCallback(std::function<void(Connection *)> &callback);

  enum class State {
    UNDEFINED,
    CONNECTING,
    CLOSED,
  };
  State getState();

  int read();
  const Buffer *getReadbuffer();

  void setSendbuffer(const uint8_t *buf, size_t size);
  int write();
  void close();

  int getLineBuffer();

  Socket *socket();

private:
  EventLoop *mEvlp = nullptr;
  std::unique_ptr<Channel> mChannel;
  std::function<void(int)> mCloseCallback;
  Buffer mReadbuf, mWritebuf;
  std::function<void(Connection *)> mReadCallback;
  State mState = State::UNDEFINED;
};

#endif