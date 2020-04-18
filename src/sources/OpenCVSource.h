#pragma once

#include <opencv2/videoio.hpp>
#include "../app/Source.h"
#include "../app/Detection.h"

class OpenCVSource : public Source<float> {
private:
    cv::VideoCapture capture;
    size_t captureHeight = 0;
    size_t captureWidth = 0;

public:
    OpenCVSource();

    Frame<float> *fetch() override;

    void writeToFile(const std::string &filename, const Frame<float> *frame,
                     const std::vector<Detection> &detections) override;

    void open(const std::string &sourceId = "");

    void close();

private:
    const cv::Mat &resize(const cv::Mat &frame, size_t width, size_t height) const;
};