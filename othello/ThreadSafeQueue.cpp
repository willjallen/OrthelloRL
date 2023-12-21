#include "ThreadSafeQueue.h"
#include "NNet.h"
#include "PredictionItem.h"

void ThreadSafeQueue::push(PredictionItem &&data) {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.emplace(std::move(data));
    cv_.notify_one();
}

PredictionItem ThreadSafeQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this](){ return !queue_.empty(); });
    PredictionItem item(std::move(queue_.front()));
    queue_.pop();
    return item;
}

size_t ThreadSafeQueue::size() {
    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
    lock.lock();
    size_t size = queue_.size();
    lock.unlock();
    return size;
}

