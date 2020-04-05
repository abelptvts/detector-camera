#include <fstream>
#include <queue>
#include "YoloV2Detector.h"
#include "../util/json.hpp"
#include "../util/math.hpp"

YoloV2Detector::YoloV2Detector(
        const std::string &modelPath,
        const std::string &metaPath) : TFLiteDetector<float>(modelPath) {

    using json = nlohmann::json;

    std::ifstream metaFile(metaPath);
    std::stringstream stream;
    stream << metaFile.rdbuf();

    auto metaString = stream.str();
    auto metaData = json::parse(metaString);

    if (!metaData.contains("labels")) {
        throw DetectorAppException("yolov2-tiny metadata json file does not contain 'labels'");
    }
    for (const auto &it : metaData["labels"]) {
        this->labels.push_back(it);
    }

    if (!metaData.contains("anchors")) {
        throw DetectorAppException("yolov2-tiny metadata json file does not contain 'anchors'");
    }
    for (const auto &it : metaData["anchors"]) {
        this->anchors.push_back(it);
    }
}

std::vector<Detection> YoloV2Detector::postProcess() const {
    std::cout << "Post processing" << std::endl;

    std::vector<Detection> result;

    auto compareDetections = [](Detection &d1, Detection &d2) {
        return d1.confidence < d2.confidence;
    };
    std::priority_queue<Detection, std::vector<Detection>, decltype(compareDetections)> pqDetections(compareDetections);

    auto *outputDims = this->interpreter->tensor(this->outputTensorIdx)->dims->data;
    auto *output = this->interpreter->typed_tensor<float>(this->outputTensorIdx);

    const size_t NUM_BLOCKS_Y = outputDims[1];
    const size_t NUM_BLOCKS_X = outputDims[2];
    const size_t BLOCK_WIDTH = this->requiredWidth / NUM_BLOCKS_X;
    const size_t BLOCK_HEIGHT = this->requiredHeight / NUM_BLOCKS_Y;
    const size_t BOXES_PER_BLOCK = this->anchors.size() / 2;
    const size_t NUM_CLASSES = this->labels.size();
    const size_t OUTPUT_SIZE_PER_BOX = (5 + NUM_CLASSES);

    if (outputDims[3] != BOXES_PER_BLOCK * (5 + NUM_CLASSES)) {
        throw DetectorAppException("tflite output size is different than expected");
    }

    for (size_t y = 0; y < NUM_BLOCKS_Y; ++y) {
        for (size_t x = 0; x < NUM_BLOCKS_X; ++x) {
            for (size_t b = 0; b < BOXES_PER_BLOCK; ++b) {
                const size_t offset = NUM_BLOCKS_X * BOXES_PER_BLOCK * OUTPUT_SIZE_PER_BOX * y +
                                      BOXES_PER_BLOCK * OUTPUT_SIZE_PER_BOX * x +
                                      OUTPUT_SIZE_PER_BOX * b;

                const float boxX = (x + sigmoid(output[offset + 0])) * BLOCK_WIDTH;
                const float boxY = (y + sigmoid(output[offset + 1])) * BLOCK_HEIGHT;
                const float boxW = std::exp(output[offset + 2]) * this->anchors[2 * b + 0] * BLOCK_WIDTH;
                const float boxH = std::exp(output[offset + 3]) * this->anchors[2 * b + 1] * BLOCK_HEIGHT;
                float confidence = sigmoid(output[offset + 4]);

                std::vector<float> classes(NUM_CLASSES);
                for (int c = 0; c < NUM_CLASSES; ++c) {
                    classes[c] = output[offset + 5 + c];
                }
                softmax(classes);

                size_t classIdx = 0;
                for (int c = 0; c < NUM_CLASSES; ++c) {
                    if (classes[c] > classes[classIdx]) {
                        classIdx = c;
                    }
                }
                confidence *= classes[classIdx];

                if (confidence < this->THRESHOLD || this->labels[classIdx] != "person") {
                    continue;
                }

                Detection detection{
                        boxX,
                        boxY,
                        boxW,
                        boxH,
                        this->labels[classIdx],
                        confidence
                };

                pqDetections.push(detection);
            }
        }
    }

    while (!pqDetections.empty()) {
        result.push_back(pqDetections.top());
        pqDetections.pop();
    }

    return result;
}
