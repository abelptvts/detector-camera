#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    size_t maxSize = 30;
    std::mutex mutex;
    std::condition_variable conditionVariable;
public:
    ThreadSafeQueue(size_t size) : maxSize(size) {}

    T pop_front();

    void push(T &value);

    bool empty() const;
};

#include "ThreadSafeQueue.tpp"