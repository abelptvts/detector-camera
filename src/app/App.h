#pragma once

#include <atomic>
#include <thread>
#include "Detector.h"
#include "Source.h"
#include "../util/ThreadSafeQueue.h"
#include "Api.h"
#include "Config.h"

template<typename T>
class App {
private:
    Source<T> *source;
    Detector<T> *detector;
    Api *api;
    Config config;

    std::atomic<bool> running{};
    ThreadSafeQueue<Frame<T> *> queue;
    std::thread fetchThread;
    std::thread detectionThread;

    std::chrono::system_clock::time_point lastDetectionTime = std::chrono::system_clock::now();

    float avgDetectionTime = 0;
    unsigned long long totalDetections = 0;

    void fetchTask();

    void detectionTask();

public:
    App(Source<T> *src, Detector<T> *det, Api *api, Config &config) : source(src), detector(det),
                                                                      api(api), queue(config.QUEUE_MAX_ITEMS),
                                                                      config(config) {}

    void start();

    void stop();

    void oneShot();

    float getAvgDetectionTime() {
        return this->avgDetectionTime;
    }
};

#include "App.tpp"