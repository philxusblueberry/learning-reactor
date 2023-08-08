#include "InetAddress.h"
#include <cstring>

InetAddress::InetAddress(const char *ip, int port) {
  memset(&mAddr, 0, sizeof(mAddr));
  mAddr.sin_family = AF_INET;
  mAddr.sin_addr.s_addr = !strcmp(ip, "") ? INADDR_ANY : inet_addr(ip);
  mAddr.sin_port = htons(port);
}

sockaddr *InetAddress::getSockAddr() { return (sockaddr *)&mAddr; }

int InetAddress::getSockAddrSize() { return sizeof(mAddr); }