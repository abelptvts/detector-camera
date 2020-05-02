#pragma once

#include "TFLiteDetector.h"

class YoloV2Detector : public TFLiteDetector<float> {
private:
    std::vector<std::string> labels;
    std::vector<float> anchors;
    float THRESHOLD = .5f;
public:
    explicit YoloV2Detector(const std::string &modelPath, const std::string &metaPath, int numThreads = -1,
                            float precisionThreshold = .5f);

protected:
    std::vector<Detection> postProcess() const override;
};