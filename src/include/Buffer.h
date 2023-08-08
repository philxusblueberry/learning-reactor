#ifndef __BUFFER__
#define __BUFFER__

#include <string>

class Buffer {
public:
  const uint8_t *getBuf() const;
  size_t append(const uint8_t *buf, size_t size);
  size_t size() const;
  void clear();
  size_t getLine();
  const char *getStr() const;

private:
  std::string mBuf;
};

#endif