#pragma once

#include <string>

typedef struct Detection {
    float x;
    float y;
    float width;
    float height;
    std::string className;
    float confidence;
} Detection;