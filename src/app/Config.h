#pragma once

#include <string>

struct Config {
    float DETECTION_PRECISION_THRESHOLD = .5f;
    std::string CAPTURE_URL;
    std::string YOLOV2_TINY_MODEL_PATH = "../assets/yolov2-tiny/";
    int NUM_DETECTION_THREADS = 2;
    int SLEEP_BETWEEN_DETECTIONS = 100;
    int SLEEP_BETWEEN_FRAME_FETCHES = 100;
    int QUEUE_MAX_ITEMS = 30;
    int MIN_TIME_BETWEEN_NOTIFICATIONS = 2000;
    std::string API_HOSTNAME;
    int API_PORT;
    std::string TOKEN;
};