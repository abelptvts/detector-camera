#pragma once

#include <cmath>
#include <vector>

float sigmoid(float x) {
    return 1 / (1 + std::exp(-x));
}

float fastSigmoid(float x) {
    return x / (1 + std::abs(x));
}

void softmax(std::vector<float> &data) {
    float exponentials[data.size()];
    float sum = 0;
    for (int i = 0; i < data.size(); ++i) {
        exponentials[i] = std::exp(data[i]);
        sum += exponentials[i];
    }

    for (int i = 0; i < data.size(); ++i) {
        data[i] = exponentials[i] / sum;
    }
}