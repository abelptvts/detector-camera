
#include "ThreadSafeQueue.h"
#include "ThreadSafeQueueExeption.h"

template<typename T>
T ThreadSafeQueue<T>::pop_front() {
    std::unique_lock<std::mutex> lock(this->mutex);
    while (this->queue.empty()) {
        if (this->conditionVariable.wait_for(lock, std::chrono::milliseconds(5000)) == std::cv_status::timeout) {
            throw ThreadSafeQueueException("thread-safe queue pop timeout");
        }
        this->conditionVariable.wait(lock);
    }

    auto val = this->queue.front();
    this->queue.pop();
    this->conditionVariable.notify_one();
    lock.unlock();
    return val;
}

template<typename T>
void ThreadSafeQueue<T>::push(T &value) {
    std::unique_lock<std::mutex> lock(this->mutex);
    if (this->queue.size() == this->maxSize) {
        if (this->conditionVariable.wait_for(lock, std::chrono::milliseconds(5000)) == std::cv_status::timeout) {
            throw ThreadSafeQueueException("thread-safe queue push timeout");
        }
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
