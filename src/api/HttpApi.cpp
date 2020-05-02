#include "HttpApi.h"
#include "../app/DetectorAppException.h"
#include "../util/json.hpp"

#include <utility>
#include <iomanip>

HttpApi::HttpApi(std::string baseUrl, int port, std::string token)
        : Api(), baseUrl(std::move(baseUrl)), port(port), token(std::move(token)) {
    if (this->port == 443) {
        this->httpClient = new httplib::SSLClient(this->baseUrl, this->port);
    } else {
        this->httpClient = new httplib::Client(this->baseUrl, this->port);
    }
    this->httpClient->set_timeout_sec(3);
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
    if (res->status != 201) {
        throw DetectorAppException("API error: " + res->body);
    }
}

bool HttpApi::getShouldStart() {
    httplib::Headers headers = {
            {"Authorization", "Bearer " + this->token}
    };
    auto res = this->httpClient->Get("/api/cameras/me", headers);
    if (res == nullptr) {
        throw DetectorAppException("API response is null");
    }
    if (res->status != 200) {
        throw DetectorAppException("API error: " + res->body);
    }

    using json = nlohmann::json;
    auto body = json::parse(res->body);

    return body["enabled"];
}

