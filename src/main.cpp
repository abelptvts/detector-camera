#define DEBUG

#include <iostream>
#include <csignal>

#include "detectors/YoloV2Detector.h"
#include "sources/OpenCVSource.h"
#include "app/App.h"
#include "app/DetectorAppException.h"
#include "api/HttpApi.h"

bool running = true;

int main(int argc, char **argv) {
    std::string modelPath = "../assets/yolov2-tiny.tflite";
    std::string metaPath = "../assets/yolov2-tiny.meta";

    try {
        YoloV2Detector detector(modelPath, metaPath);
        OpenCVSource source;
        HttpApi api("localhost:3000",
                    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6NCwicm9sZSI6ImNhbWVyYSIsImlhdCI6MTU4Njg5NDMyMn0.MxUlnZ7IesDpVBFWfgisga8YueBGeoEUvy2TrHW60F4");

        source.setRequiredChannels(detector.getRequiredChannels());
        source.setRequiredHeight(detector.getRequiredHeight());
        source.setRequiredWidth(detector.getRequiredWidth());

        source.open();

        App<float> app(&source, &detector, &api);

        if (argc > 1 && strcmp(argv[1], "oneshot") == 0) {
            app.oneShot();
            return 0;
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