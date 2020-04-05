#pragma once

#include <vector>
#include "Detection.h"
#include "Frame.h"

template<typename T>
class Detector {
public:
    virtual std::vector<Detection> detect(Frame<T> &frame) const = 0;
};