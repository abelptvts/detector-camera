#pragma once

#include "../app/Detector.h"
#include "tensorflow/lite/model.h"

template<typename T>
class TFLiteDetector : public Detector<T> {
protected:
    std::unique_ptr<tflite::FlatBufferModel> model;
    std::unique_ptr<tflite::Interpreter> interpreter;
    int inputTensorIdx = 0;
    int outputTensorIdx = 0;
    int requiredHeight = 0;
    int requiredWidth = 0;
    int requiredChannels = 0;

public:
    int getRequiredHeight() const;

    int getRequiredWidth() const;

    int getRequiredChannels() const;

    explicit TFLiteDetector(const std::string &modelPath);

    std::vector<Detection> detect(Frame<T> &frame) const override;

protected:
    virtual std::vector<Detection> postProcess() const = 0;
};

#include "TFLiteDetector.tpp"