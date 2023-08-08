#include "Buffer.h"
#include <iostream>

const uint8_t *Buffer::getBuf() const { return (const uint8_t *)mBuf.data(); }

size_t Buffer::append(const uint8_t *buf, size_t size) {
  mBuf.append((const char *)buf, size);
  return mBuf.size();
}

size_t Buffer::size() const { return mBuf.size(); }

void Buffer::clear() { mBuf.clear(); }

size_t Buffer::getLine() {
  std::getline(std::cin, mBuf);
  return mBuf.size();
}

const char *Buffer::getStr() const { return mBuf.c_str(); }