#ifndef __MY_SOCKET__
#define __MY_SOCKET__
#include <stddef.h>

class InetAddress;

class Socket {
public:
  Socket();

  Socket(int fd);

  ~Socket();

  Socket(const Socket &sock) = delete;

  Socket(Socket &&sock) = delete;

  Socket &operator=(const Socket &sock) = delete;

  Socket &operator=(Socket &&sock) = delete;

  int bind(InetAddress *addr);

  int connect(InetAddress *addr);

  int listen();

  Socket *accept();

  int close();

  int write(const char *buf, size_t size);

  int read(char *buf, size_t size);

  int setnonblocking();

  int getFd();

private:
  int fd = -1;
};

#endif