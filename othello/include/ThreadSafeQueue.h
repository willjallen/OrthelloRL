#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>


#include "PredictionItem.h"


class ThreadSafeQueue {
  public:
      void push(PredictionItem &&data);
      PredictionItem pop();
      size_t size();

  private:
      std::queue<PredictionItem> queue_;
      std::mutex mutex_;
      std::condition_variable cv_;
};

#endif

