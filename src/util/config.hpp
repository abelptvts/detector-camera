#pragma once


#include <string>
#include <fstream>
#include "../app/Config.h"
#include "../app/DetectorAppException.h"

void parseConfig(std::string &path, Config &config) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw DetectorAppException("could not open config file");
    }

    std::string line;
    while (file >> line) {
        if (line[0] == '#' || line.empty()) {
            continue;
        }

        auto delimiterIdx = line.find('=');
        auto key = line.substr(0, delimiterIdx);
        auto value = line.substr(delimiterIdx + 1, line.length());

        if (key == "NUM_DETECTION_THREADS") {
            config.NUM_DETECTION_THREADS = std::stoi(value);
            continue;
        }
        if (key == "SLEEP_BETWEEN_DETECTIONS") {
            config.SLEEP_BETWEEN_DETECTIONS = std::stoi(value);
            continue;
        }
        if (key == "SLEEP_BETWEEN_FRAME_FETCHES") {
            config.SLEEP_BETWEEN_FRAME_FETCHES = std::stoi(value);
            continue;
        }
        if (key == "QUEUE_MAX_ITEMS") {
            config.QUEUE_MAX_ITEMS = std::stoi(value);
            continue;
        }
        if (key == "MIN_TIME_BETWEEN_NOTIFICATIONS") {
            config.MIN_TIME_BETWEEN_NOTIFICATIONS = std::stoi(value);
            continue;
        }
        if (key == "API_HOSTNAME") {
            config.API_HOSTNAME = value;
            continue;
        }
        if (key == "API_PORT") {
            config.API_PORT = std::stoi(value);
            continue;
        }
        if (key == "TOKEN") {
            config.TOKEN = value;
            continue;
        }

        throw DetectorAppException("invalid configuration: " + key);
    }
}