#pragma once

#include <iostream>
#include <utility>
#include "../app/Api.h"
#include "../util/httplib.h"

class HttpApi : public Api {
private:
    std::string baseUrl;
    int port;
    std::string token;

    httplib::Client *httpClient;

public:
    HttpApi(std::string baseUrl, int port, std::string token);

    ~HttpApi() {
        delete this->httpClient;
    }

    void
    createDetection(std::string pathToCapture, std::chrono::system_clock::time_point &date) override;
};