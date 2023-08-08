#include "Buffer.h"
#include "Connection.h"
#include "InetAddress.h"
#include "Socket.h"
#include <cstring>
#include <iostream>
#include <stdio.h>

int main() {
  Socket *sock = new Socket();
  InetAddress addr("", 23323);
  sock->connect(&addr);
  Connection *conn = new Connection(nullptr, sock);
  while (true) {
    conn->getLineBuffer();
    conn->write();
    if (conn->getState() == Connection::State::CLOSED) {
      conn->close();
      break;
    }
    conn->read();
    std::cout << "Message from server: " << conn->getReadbuffer()->getStr()
              << std::endl;
  }
  delete conn;
  return 0;
}