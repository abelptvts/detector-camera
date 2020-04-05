#pragma once

template<typename T>
struct Frame {
    int width;
    int height;
    int channels;
    T *data;
};
