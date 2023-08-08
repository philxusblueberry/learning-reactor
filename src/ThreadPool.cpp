#include "ThreadPool.h"
#include "common.h"

ThreadPool::ThreadPool(int size) {
  if (size < 0)
    size = std::thread::hardware_concurrency();
  printf("xxl check thread pool size %d\n", size);
  errif(size <= 0 || size > 200, "xxl check hardware support error");
  for (int i = 0; i < size; ++i) {
    mThreads.emplace_back([this]() -> void {
      while (true) {
        Task cur;
        {
          std::unique_lock<std::mutex> lck(mTasksMutex);
          mCond.wait(lck,
                     [this]() -> bool { return mQuit || !mTasks.empty(); });
          if (mQuit) {
            return;
          }
          swap(cur, mTasks.front());
          mTasks.pop_front();
        }
        cur();
      }
    });
  }
}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::stop() {
  {
    std::lock_guard<std::mutex> lck(mTasksMutex);
    mQuit = true;
  }
  mCond.notify_all();
  for (auto &&thread : mThreads) {
    if (thread.joinable())
      thread.join();
  }
}
