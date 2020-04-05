#include <iostream>
#include <thread>
#include <chrono>

#include "App.h"


template<typename T>
void App<T>::start() {
    this->running = true;
    this->fetchThread = std::thread([&]() {
        this->fetchTask();
    });
    this->detectionThread = std::thread([&]() {
        this->detectionTask();
    });
}

template<typename T>
void App<T>::stop() {
    this->running = false;
    this->fetchThread.join();
    this->detectionThread.join();
}

template<typename T>
void App<T>::oneShot() {
    auto frame = this->source->fetch();
    auto detections = this->detector->detect(frame);
    this->source->writeToFile("out.png", frame, detections);
}

template<typename T>
void App<T>::fetchTask() {
    while (this->running) {
        auto frame = this->source->fetch();
        this->queue.push(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

template<typename T>
void App<T>::detectionTask() {
    while (this->running) {
        auto frame = this->queue.pop_front();
        auto detections = this->detector->detect(frame);
        this->source->writeToFile("out.png", frame, detections);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
