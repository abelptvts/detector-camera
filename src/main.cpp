#define DEBUG

#include <iostream>
#include <csignal>

#include "detectors/YoloV2Detector.h"
#include "sources/OpenCVSource.h"
#include "app/App.h"
#include "app/DetectorAppException.h"

bool running = true;

int main() {
    std::string modelPath = "../assets/yolov2-tiny.tflite";
    std::string metaPath = "../assets/yolov2-tiny.meta";
    try {
        YoloV2Detector detector(modelPath, metaPath);
        OpenCVSource source;

        source.setRequiredChannels(detector.getRequiredChannels());
        source.setRequiredHeight(detector.getRequiredHeight());
        source.setRequiredWidth(detector.getRequiredWidth());

        source.open();

        App<float> app(&source, &detector);

        app.start();

        signal(SIGTERM, [](int signum) {
            running = false;
        });

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Stopping" << std::endl;

        app.stop();

        source.close();
    } catch (DetectorAppException &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}