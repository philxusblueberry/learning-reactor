#ifndef __SERVER__
#define __SERVER__

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

class EventLoop;
class Connection;
class Acceptor;
class Socket;
class ThreadPool;

class Server {
public:
  Server();
  ~Server();
  Server(const Server &sv) = delete;
  Server(Server &&sv) = delete;
  Server &operator=(const Server &sv) = delete;
  Server &operator=(Server &&sv) = delete;

  void start();

  void newConnection(Socket *sock);
  void deleteConnection(int fd);

  void setConnectCallback(std::function<void(Connection *)> &&callback);
  void setHandleCallback(std::function<void(Connection *)> &&callback);

private:
  std::unique_ptr<EventLoop> mMainReactor;
  std::unique_ptr<Acceptor> mAcceptor;
  std::mutex mMutex;
  std::unordered_map<int, std::unique_ptr<Connection>> records;
  std::vector<std::unique_ptr<EventLoop>> mSubReactors;
  std::unique_ptr<ThreadPool> mPool;
  std::function<void(Connection *)> mConnectCallback;
  std::function<void(Connection *)> mHandleCallback;
  std::unique_ptr<Connection> mConnectionHack;
};

#endif