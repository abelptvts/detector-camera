#define DEBUG

#include <iostream>
#include <csignal>

#include "detectors/YoloV2Detector.h"
#include "sources/OpenCVSource.h"
#include "app/App.h"
#include "app/DetectorAppException.h"
#include "api/HttpApi.h"
#include "util/config.hpp"

bool running = true;

int main(int argc, char **argv) {
    std::string modelPath = "../assets/yolov2-tiny.tflite";
    std::string metaPath = "../assets/yolov2-tiny.meta";
    std::string configPath = "../detector_camera.conf";

    try {
        Config config;
        YoloV2Detector detector(modelPath, metaPath, config.NUM_DETECTION_THREADS);
        OpenCVSource source;
        HttpApi api(config.API_HOSTNAME, config.API_PORT, config.TOKEN);

        source.setRequiredChannels(detector.getRequiredChannels());
        source.setRequiredHeight(detector.getRequiredHeight());
        source.setRequiredWidth(detector.getRequiredWidth());

        source.open();

        App<float> app(&source, &detector, &api, config);

        if (argc > 2 && strcmp(argv[1], "--config") == 0) {
            std::string path(argv[2]);
            parseConfig(path, config);
        }

        app.start();

        signal(SIGINT, [](int signum) {
            running = false;
        });

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        app.stop();
        std::cout << "Average detection time: " << app.getAvgDetectionTime() << " ms" << std::endl;

        source.close();
    } catch (DetectorAppException &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}