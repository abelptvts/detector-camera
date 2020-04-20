#include "HttpApi.h"
#include "../app/DetectorAppException.h"

#include <utility>
#include <iomanip>

HttpApi::HttpApi(std::string baseUrl, std::string token) : Api(), baseUrl(std::move(baseUrl)), token(std::move(token)) {
    auto delimIdx = this->baseUrl.find(':');
    auto hostname = this->baseUrl.substr(0, delimIdx);
    auto port = std::stoi(this->baseUrl.substr(delimIdx + 1, this->baseUrl.length()));

    this->httpClient = new httplib::Client(hostname, port);
}

std::string toISOString(std::chrono::system_clock::time_point date) {
    auto itt = std::chrono::system_clock::to_time_t(date);
    std::ostringstream ss;
    ss << std::put_time(gmtime(&itt), "%FT%TZ");
    return ss.str();
}

void
HttpApi::createDetection(std::string pathToCapture, std::chrono::system_clock::time_point &date) {
    std::ifstream file(pathToCapture, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw DetectorAppException("could not open capture file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    httplib::MultipartFormDataItems items = {
            {"capture", buffer.str(), "out.png", "image/png"},
            {"date",    toISOString(date)}
    };

    httplib::Headers headers = {
            {"Authorization", "Bearer " + this->token}
    };
    auto res = this->httpClient->Post("/api/detections/", headers, items);
    if (res == nullptr) {
        throw DetectorAppException("API response is null");
    }
    std::cout << res->body << std::endl;
}

