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
    auto *frame = this->source->fetch();
    auto start = std::chrono::high_resolution_clock::now();
    auto detections = this->detector->detect(frame);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Detecting took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    this->source->writeToFile("out.png", frame, detections);
    delete frame;
}

template<typename T>
void App<T>::fetchTask() {
    while (this->running) {
        auto *frame = this->source->fetch();
        this->queue.push(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    }
}

template<typename T>
void App<T>::detectionTask() {
    while (this->running) {
        auto *frame = this->queue.pop_front();
        std::cout << "Detecting..." << std::endl;
        auto detections = this->detector->detect(frame);
        std::cout << "DONE!" << std::endl;
        this->source->writeToFile("out.png", frame, detections);

        delete frame;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
