#pragma once

#include <string>

struct Config {
    int NUM_DETECTION_THREADS = 2;
    int SLEEP_BETWEEN_DETECTIONS = 100;
    int SLEEP_BETWEEN_FRAME_FETCHES = 100;
    int QUEUE_MAX_ITEMS = 30;
    int MIN_TIME_BETWEEN_NOTIFICATIONS = 2000;
    std::string API_HOSTNAME = "localhost";
    int API_PORT = 3000;
    std::string TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6NCwicm9sZSI6ImNhbWVyYSIsImlhdCI6MTU4Njg5NDMyMn0.MxUlnZ7IesDpVBFWfgisga8YueBGeoEUvy2TrHW60F4";
};