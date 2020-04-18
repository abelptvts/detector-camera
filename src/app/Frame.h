#pragma once

template<typename T>
struct Frame {
    int width;
    int height;
    int channels;
    std::vector<T> data;
};
