#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include "OpenCVSource.h"
#include "../app/DetectorAppException.h"

OpenCVSource::OpenCVSource() {
    this->capture = cv::VideoCapture();
}

Frame<float> *OpenCVSource::fetch() {
    cv::Mat capturedFrame;
    this->capture >> capturedFrame;

    const auto resized = this->resize(capturedFrame, this->requiredWidth, this->requiredHeight);

    if (capturedFrame.channels() != this->requiredChannels) {
        throw DetectorAppException("capturedFrame number of channels does not match required number of channels");
    }

    auto *frame = new Frame<float>();
    frame->data = std::vector<float>(this->requiredWidth * this->requiredHeight * this->requiredChannels);

    size_t index = 0;
    for (int y = 0; y < this->requiredHeight; ++y) {
        for (int x = 0; x < this->requiredWidth; ++x) {
            index = (y * this->requiredWidth + x) * this->requiredChannels;
            for (int c = 0; c < this->requiredChannels; ++c) {
                // opencv returns frames in BGR format
                frame->data[index + c] = resized.data[index + (this->requiredChannels - c - 1)] / 255.0f;
            }
        }
    }

    auto now = std::chrono::system_clock::now();
    frame->channels = this->requiredChannels;
    frame->height = this->requiredHeight;
    frame->width = this->requiredWidth;
    frame->date = now;

    return frame;
}

const cv::Mat &OpenCVSource::resize(const cv::Mat &frame, const size_t width, const size_t height) const {
    static cv::Mat resized;
    cv::resize(frame, resized, cv::Size(width, height));
    return resized;
}

void OpenCVSource::open(const std::string &sourceId) {
    if (!sourceId.empty()) {
        this->capture.open(sourceId);
    } else {
        this->capture.open(0);
    }
    if (!this->capture.isOpened()) {
        throw DetectorAppException("could not open source: " + sourceId);
    }

    this->captureHeight = this->capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    this->captureWidth = this->capture.get(cv::CAP_PROP_FRAME_WIDTH);

    std::cout << "Video Capture opened with resolution: " << this->captureWidth << "x" << this->captureHeight
              << std::endl;
}

void OpenCVSource::close() {
    this->capture.release();
}

void OpenCVSource::writeToFile(const std::string &filename, const Frame<float> *frame,
                               const std::vector<Detection> &detections) {
    cv::Mat picture(frame->height, frame->width, CV_8UC3);
    size_t index = 0;
    for (int y = 0; y < frame->height; ++y) {
        for (int x = 0; x < frame->width; ++x) {
            index = (y * frame->width + x) * frame->channels;
            for (int c = 0; c < frame->channels; ++c) {
                // opencv returns frames in BGR format
                picture.data[index + c] = frame->data[index + (frame->channels - c - 1)] * 255;
            }
        }
    }

    for (const auto &it : detections) {
        cv::Rect rect(it.x - it.width / 2, it.y - it.height / 2, it.width, it.height);
        cv::rectangle(picture, rect, cv::Scalar(0, 255, 0));
    }

    const auto resized = this->resize(picture, this->captureWidth, this->captureHeight);

    cv::imwrite(filename, resized);
}


