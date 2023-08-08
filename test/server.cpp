#include "Server.h"
#include "Buffer.h"
#include "Connection.h"
#include "Socket.h"
#include <iostream>
#include <memory>

int main() {
  std::unique_ptr<Server> serv = std::make_unique<Server>();
  serv->setConnectCallback([](Connection *con) -> void {
    std::cout << "accept client with port " << con->socket()->getFd()
              << std::endl;
  });
  serv->setHandleCallback([](Connection *con) -> void {
    if (con->getState() == Connection::State::CONNECTING) {
      const Buffer *readBuf = con->getReadbuffer();
      con->setSendbuffer(readBuf->getBuf(), readBuf->size());
      con->write();
    }
  });
  serv->start();
  return 0;
}