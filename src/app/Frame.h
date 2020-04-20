#pragma once

#include <chrono>
#include <vector>

template<typename T>
struct Frame {
    int width;
    int height;
    int channels;
    std::chrono::system_clock::time_point date;
    std::vector<T> data;
};
