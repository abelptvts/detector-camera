#pragma once

#include <bits/unique_ptr.h>
#include "Frame.h"
#include "Detection.h"

template<typename T>
class Source {
protected:
    int requiredWidth = 0;
    int requiredHeight = 0;
    int requiredChannels = 0;

public:
    void setRequiredWidth(int width) {
        this->requiredWidth = width;
    }

    void setRequiredHeight(int height) {
        this->requiredHeight = height;
    }

    void setRequiredChannels(int channels) {
        this->requiredChannels = channels;
    }

public:
    virtual Frame<T> *fetch() = 0;

    virtual void
    writeToFile(const std::string &filename, const Frame<T> *frame, const std::vector<Detection> &detections) = 0;
};