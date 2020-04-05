#pragma once

#include <atomic>
#include <thread>
#include "Detector.h"
#include "Source.h"
#include "../util/ThreadSafeQueue.h"

template<typename T>
class App {
private:
    Source<T> *source;
    Detector<T> *detector;

    std::atomic<bool> running{};
    ThreadSafeQueue<Frame<T> > queue;
    std::thread fetchThread;
    std::thread detectionThread;

    void fetchTask();

    void detectionTask();

public:
    App(Source<T> *src, Detector<T> *det) : source(src), detector(det), queue(30) {}

    void start();
    void stop();

    void oneShot();
};

#include "App.tpp"