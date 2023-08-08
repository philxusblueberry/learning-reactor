#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
  using Task = std::function<void()>;
  ThreadPool(int size = -1);
  ~ThreadPool();
  ThreadPool(const ThreadPool &tp) = delete;
  ThreadPool(ThreadPool &&tp) = delete;
  ThreadPool &operator=(const ThreadPool &tp) = delete;
  ThreadPool &operator=(ThreadPool &&tp) = delete;

  template <class F, class... Arg>
  auto add(F &&f, Arg &&...args)
      -> std::future<typename std::result_of<F(Arg...)>::type>;

  void stop();

private:
  std::vector<std::thread> mThreads;
  bool mQuit = false;
  std::list<Task> mTasks;
  std::mutex mTasksMutex;
  std::condition_variable mCond;
};

template <class F, class... Arg>
auto ThreadPool::add(F &&f, Arg &&...args)
    -> std::future<typename std::result_of<F(Arg...)>::type> {
  using returnType = typename std::result_of<F(Arg...)>::type;

  auto task = std::make_shared<std::packaged_task<returnType()>>(
      std::bind(std::forward<F>(f), std::forward<Arg>(args)...));

  auto res = task->get_future();

  {
    std::unique_lock<std::mutex> lck(mTasksMutex);
    if (mQuit)
      return std::future<returnType>();
    mTasks.emplace_back([task]() -> void { (*task)(); });
  }
  mCond.notify_one();

  return res;
}

#endif