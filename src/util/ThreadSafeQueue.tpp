
#include "ThreadSafeQueue.h"

template<typename T>
T ThreadSafeQueue<T>::pop_front() {
    std::unique_lock<std::mutex> lock(this->mutex);
    while (this->queue.empty()) {
        this->conditionVariable.wait(lock);
    }

    auto val = this->queue.front();
    this->queue.pop();
    if(this->queue.size() < this->maxSize) {
        this->conditionVariable.notify_one();
    }
    lock.unlock();
    return val;
}

template<typename T>
void ThreadSafeQueue<T>::push(T &value) {
    std::unique_lock<std::mutex> lock(this->mutex);
    if (this->queue.size() >= this->maxSize) {
        this->conditionVariable.wait(lock);
    }

    this->queue.push(value);
    lock.unlock();
    this->conditionVariable.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->queue.empty();
}
