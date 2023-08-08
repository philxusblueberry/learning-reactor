#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Server.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop *evlp) : mEp(evlp) {
  Socket *serverSock = new Socket();
  InetAddress *addr = new InetAddress("", 23323);
  serverSock->bind(addr);
  serverSock->listen();
  delete addr;

  // serverSock->setnonblocking();
  mAcceptChannel = new Channel(evlp, serverSock);
  mAcceptChannel->enableReading();
  // mAcceptChannel->enableET();
  mAcceptChannel->setReadCallBack(std::bind(&Acceptor::acception, this));
}

Acceptor::~Acceptor() { close(); }

int Acceptor::close() {
  mEp = nullptr;
  delete mAcceptChannel->getSocket();
  delete mAcceptChannel;
  return 0;
}

void Acceptor::acception() {
  if (mCallback) {
    // printf("xxl check %s %d\n", __FUNCTION__, __LINE__);
    mCallback(mAcceptChannel->getSocket());
  }
}

int Acceptor::setNewConnectionCallback(std::function<void(Socket *)> &&func) {
  mCallback = func;
  return 0;
}