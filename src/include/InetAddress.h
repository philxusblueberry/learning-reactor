#ifndef __INET_ADDRESS__
#define __INET_ADDRESS__

#include <arpa/inet.h>

class InetAddress {
public:
  InetAddress() = default;

  InetAddress(const char *ip, int port);

  sockaddr *getSockAddr();

  int getSockAddrSize();

private:
  sockaddr_in mAddr;
};

#endif