#include <tensorflow/lite/kernels/register.h>
#include "../app/DetectorAppException.h"
#include <iostream>

#ifdef DEBUG

#include <tensorflow/lite/optional_debug_tools.h>

#endif

template<typename T>
TFLiteDetector<T>::TFLiteDetector(const std::string &modelPath): Detector<T>() {
    this->model = tflite::FlatBufferModel::BuildFromFile(modelPath.c_str());
    if (this->model == nullptr) {
        throw DetectorAppException("tflite model file could not be loaded");
    }

    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder interpreterBuilder(*(this->model), resolver);
    interpreterBuilder(&(this->interpreter));
    this->interpreter->SetNumThreads(2);
    if (this->interpreter->AllocateTensors() != kTfLiteOk) {
        throw DetectorAppException("could not allocate interpreter tensors");
    }

    const size_t numInputTensors = this->interpreter->inputs().size();
    const size_t numOutputTensors = this->interpreter->outputs().size();
    if (numInputTensors == 0) {
        throw DetectorAppException("Number of input tensors is 0.");
    }
    if (numOutputTensors == 0) {
        throw DetectorAppException("Number of output tensors is 0.");
    }

    // we're doing object detection, assume one input and one output tensor
    this->inputTensorIdx = this->interpreter->inputs()[0];
    this->outputTensorIdx = this->interpreter->outputs()[0];

    auto *inputTensor = this->interpreter->tensor(this->inputTensorIdx);
    this->requiredHeight = inputTensor->dims->data[1];
    this->requiredWidth = inputTensor->dims->data[2];
    this->requiredChannels = inputTensor->dims->data[3];
}

template<typename T>
std::vector<Detection> TFLiteDetector<T>::detect(Frame<T> *frame) const {
    auto *input = this->interpreter->template typed_tensor<T>(this->inputTensorIdx);
    if (input == nullptr) {
        throw DetectorAppException("input tensor is nullptr");
    }

    size_t index = 0;
    for (int h = 0; h < this->requiredHeight; ++h) {
        for (int w = 0; w < this->requiredWidth; ++w) {
            for (int c = 0; c < this->requiredChannels; ++c) {
                input[index] = frame->data[index];
                ++index;
            }
        }
    }

    if (interpreter->Invoke() != kTfLiteOk) {
        throw DetectorAppException("inference failed");
    }

    return this->postProcess();
}

template<typename T>
int TFLiteDetector<T>::getRequiredHeight() const {
    return requiredHeight;
}

template<typename T>
int TFLiteDetector<T>::getRequiredWidth() const {
    return requiredWidth;
}

template<typename T>
int TFLiteDetector<T>::getRequiredChannels() const {
    return requiredChannels;
}