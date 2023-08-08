#include "Server.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "common.h"
#include <assert.h>
#include <memory>

Server::Server() {
  mMainReactor = std::make_unique<EventLoop>();
  mAcceptor = std::make_unique<Acceptor>(mMainReactor.get());
  mAcceptor->setNewConnectionCallback(
      std::bind(&Server::newConnection, this, std::placeholders::_1));
  int n = std::thread::hardware_concurrency();
  mPool = std::make_unique<ThreadPool>(n);
  for (int i = 0; i < n; ++i) {
    mSubReactors.emplace_back(std::make_unique<EventLoop>());
  }
}

Server::~Server() {}

void Server::start() {
  for (size_t i = 0; i < mSubReactors.size(); ++i) {
    mPool->add(&EventLoop::loop, mSubReactors[i].get());
  }
  mMainReactor->loop();
}

void Server::newConnection(Socket *sock) {
  Socket *clntSock = sock->accept();
  int i = clntSock->getFd() % mSubReactors.size();
  // to do multithread safe
  Connection *tmp = new Connection(mSubReactors[i].get(), clntSock);
  auto con = std::unique_ptr<Connection>(std::unique_ptr<Connection>(tmp));
  con->setReadCallback(mHandleCallback);
  con->setCloseCallback(
      std::bind(&Server::deleteConnection, this, std::placeholders::_1));
  {
    std::lock_guard<std::mutex> lck(mMutex);
    records[clntSock->getFd()] = std::move(con);
    if (mConnectCallback) {
      mConnectCallback(records[clntSock->getFd()].get());
    }
  }
}

void Server::deleteConnection(int fd) {
  std::lock_guard<std::mutex> lck(mMutex);
  auto iter = records.find(fd);
  if (iter == records.end()) {
    return;
  }
  // to avoid dangerous
  mConnectionHack = std::move(iter->second);
  records.erase(iter);
}

void Server::setHandleCallback(std::function<void(Connection *)> &&callback) {
  mHandleCallback = callback;
}

void Server::setConnectCallback(std::function<void(Connection *)> &&callback) {
  mConnectCallback = callback;
}