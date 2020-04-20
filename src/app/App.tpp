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
    this->source->writeToFile("capture.png", frame, detections);
    delete frame;
}

template<typename T>
void App<T>::fetchTask() {
    while (this->running) {
        try {
            auto *frame = this->source->fetch();
            this->queue.push(frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(this->config.SLEEP_BETWEEN_FRAME_FETCHES));
        } catch (ThreadSafeQueueException &e) {
            continue;
        }
    }
}

template<typename T>
void App<T>::detectionTask() {
    while (this->running) {
        try {
            auto *frame = this->queue.pop_front();

            auto startDetection = std::chrono::system_clock::now();
            auto detections = this->detector->detect(frame);
            auto endDetection = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endDetection - startDetection);

            this->avgDetectionTime *= this->totalDetections;
            this->avgDetectionTime += duration.count();
            this->totalDetections++;
            this->avgDetectionTime /= this->totalDetections;

            if (detections.empty()) {
                delete frame;
                continue;
            }

            this->source->writeToFile("capture.png", frame, detections);

            auto now = std::chrono::system_clock::now();
            auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->lastDetectionTime);
            if (seconds.count() > this->config.MIN_TIME_BETWEEN_NOTIFICATIONS) {
                this->api->createDetection("capture.png", frame->date);
                this->lastDetectionTime = now;
            }

            delete frame;

            std::this_thread::sleep_for(std::chrono::milliseconds(this->config.SLEEP_BETWEEN_DETECTIONS));
        } catch (ThreadSafeQueueException &e) {
            continue;
        }
    }
}
