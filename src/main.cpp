#define DEBUG

#include <iostream>
#include <csignal>

#include "detectors/YoloV2Detector.h"
#include "sources/OpenCVSource.h"
#include "app/App.h"
#include "app/DetectorAppException.h"
#include "api/HttpApi.h"
#include "util/config.hpp"
#include "../deps/socket.io-client-cpp/build/include/sio_client.h"

bool running = true;
auto signalHandler = [](int signum) {
    running = false;
};

int main(int argc, char **argv) {
    std::string configPath = "/etc/detector_camera/detector_camera.conf";

    try {
        Config config;

        if (argc > 2 && strcmp(argv[1], "--config") == 0) {
            configPath = argv[2];
        }

        parseConfig(configPath, config);

        std::string modelPath = config.YOLOV2_TINY_MODEL_PATH + "yolov2-tiny.tflite";
        std::string metaPath = config.YOLOV2_TINY_MODEL_PATH + "yolov2-tiny.meta";

        YoloV2Detector detector(modelPath, metaPath, config.NUM_DETECTION_THREADS);
        OpenCVSource source;
        HttpApi api(config.API_HOSTNAME, config.API_PORT, config.TOKEN);

        source.setRequiredChannels(detector.getRequiredChannels());
        source.setRequiredHeight(detector.getRequiredHeight());
        source.setRequiredWidth(detector.getRequiredWidth());

        source.open(config.CAPTURE_URL);
        std::cout << "Source FPS: " << source.getFPS() << std::endl;

        App<float> app(&source, &detector, &api, config);

        sio::client client;
        std::string socketURI = (config.API_PORT == 443 ? "https://" : "http://") + config.API_HOSTNAME + ':' +
                                std::to_string(config.API_PORT);
        std::map<std::string, std::string> headers = {{"Authorization", config.TOKEN}};
        client.connect(socketURI, {}, headers);

        bool shouldStart = api.getShouldStart();
        if (shouldStart) {
            app.start();
        }

        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        client.socket()->on("toggle", [&](sio::event &event) {
            bool enable = event.get_message()->get_bool();
            if (enable) {
                app.start();
            } else {
                app.stop();
            }
        });

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        app.stop();
        std::cout << "Average detection time: " << app.getAvgDetectionTime() << " ms" << std::endl;
        std::cout << "Total source push wait time: " << app.getTotalSourcePushWaitTime() << " seconds" << std::endl;

        source.close();
    } catch (DetectorAppException &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}