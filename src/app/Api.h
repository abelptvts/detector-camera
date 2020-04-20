#pragma once

#include <string>
#include "Frame.h"

class Api {
public:
    virtual void
    createDetection(std::string pathToCapture, std::chrono::system_clock::time_point &date) = 0;
};