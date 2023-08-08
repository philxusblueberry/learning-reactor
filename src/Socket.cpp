#include "Socket.h"
#include "InetAddress.h"
#include "common.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  errif(fd == -1, "create socket error");
}

Socket::Socket(int fd) : fd(fd) {}

Socket::~Socket() { close(); }

int Socket::bind(InetAddress *addr) {
  errif(::bind(fd, addr->getSockAddr(), addr->getSockAddrSize()) == -1,
        "bind socket error");
  return 0;
}

int Socket::connect(InetAddress *addr) {
  errif(::connect(fd, addr->getSockAddr(), addr->getSockAddrSize()) == -1,
        "connect socket error");
  return 0;
}

int Socket::listen() {
  errif(::listen(fd, SOMAXCONN) == -1, "listen socket error");
  return 0;
}

Socket *Socket::accept() {
  InetAddress *addr = new InetAddress();
  socklen_t l = addr->getSockAddrSize();
  int clnt_sockfd = ::accept(fd, addr->getSockAddr(), &l);
  Socket *clnt_sock = new Socket(clnt_sockfd);
  delete addr;
  return clnt_sock;
}

int Socket::close() {
  if (fd == -1) {
    return 0;
  }
  errif(::close(fd) == -1, "close sock error");
  fd = -1;
  return 0;
}

int Socket::write(const char *buf, size_t size) {
  return ::write(fd, buf, size);
}

int Socket::read(char *buf, size_t size) { return ::read(fd, buf, size); }

int Socket::setnonblocking() {
  if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK)) {
    printf("set n block error\n");
    return -1;
  }
  return 0;
}

int Socket::getFd() { return fd; }