#include "Connection.h"
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "functional"
#include "memory"

Connection::Connection(EventLoop *evlp, Socket *clntSock) : mEvlp(evlp) {
  clntSock->setnonblocking();
  mChannel = std::make_unique<Channel>(mEvlp, clntSock);
  if (mEvlp) {
    // to do multithread
    mChannel->enableReading();
    mChannel->enableET();
  }
  mState = State::CONNECTING;
}

Connection::~Connection() { mEvlp = nullptr; }

void Connection::handleEvent() {
  read();
  if (getState() == Connection::State::CLOSED) {
    close();
  } else {
    if (mReadCallback) {
      mReadCallback(this);
    }
  }
}

void Connection::setCloseCallback(std::function<void(int)> &&callback) {
  mCloseCallback = callback;
}

void Connection::setReadCallback(std::function<void(Connection *)> &&callback) {
  mReadCallback = callback;
  mChannel->setReadCallBack(std::bind(&Connection::handleEvent, this));
}

void Connection::setReadCallback(std::function<void(Connection *)> &callback) {
  mReadCallback = callback;
  mChannel->setReadCallBack(std::bind(&Connection::handleEvent, this));
}

Connection::State Connection::getState() { return mState; }

#define STATIC_INPUT_BUFFER_SIZE 1024
int Connection::read() {
  mReadbuf.clear();
  char buf[STATIC_INPUT_BUFFER_SIZE];
  Socket *cur = mChannel->getSocket();
  ssize_t res = 0;
  while (true) {
    ssize_t size = cur->read(buf, sizeof(buf));
    if (size > 0) {
      mReadbuf.append((const uint8_t *)buf, size);
      res += size;
    } else if (size < 0 && errno == EINTR) {
      continue;
    } else if (size < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      printf("cur accumulate message is %s\n", mReadbuf.getStr());
      break;
    } else if (size == 0) {
      printf("EOS disconnect %d\n", cur->getFd());
      mState = State::CLOSED;
      break;
    }
  }
  return res;
}

void Connection::setSendbuffer(const uint8_t *buf, size_t size) {
  mWritebuf.append(buf, size);
}

int Connection::write() {
  ssize_t total = mWritebuf.size(), cur_size = 0;
  Socket *cur = mChannel->getSocket();
  const uint8_t *buf = mWritebuf.getBuf();
  while (cur_size < total) {
    ssize_t size = cur->write((const char *)buf + cur_size, total - cur_size);
    if (size > 0) {
      cur_size += size;
    } else if (size < 0 && errno == EINTR) {
      continue;
    } else if (size < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      break;
    } else if (size == -1) {
      printf("EOS disconnect %d\n", cur->getFd());
      mState = State::CLOSED;
      break;
    }
  }
  mWritebuf.clear();
  return cur_size;
}

void Connection::close() {
  if (mCloseCallback) {
    mCloseCallback(mChannel->getSocket()->getFd());
  }
}

const Buffer *Connection::getReadbuffer() { return &mReadbuf; }

int Connection::getLineBuffer() { return mWritebuf.getLine(); }

Socket *Connection::socket() { return mChannel->getSocket(); }