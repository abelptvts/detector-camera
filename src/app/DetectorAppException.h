#pragma once

class DetectorAppException : public std::exception {
private:
    std::string reason;
public:
    DetectorAppException(std::string reason) : std::exception() {
        this->reason = reason;
    }

    virtual const char *what() const throw() {
        return reason.c_str();
    }
};