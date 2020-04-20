#pragma once

#include <exception>
#include <string>

class ThreadSafeQueueException : public std::exception {
private:
    std::string reason;
public:
    ThreadSafeQueueException(std::string reason) : std::exception() {
        this->reason = reason;
    }

    virtual const char *what() const throw() {
        return reason.c_str();
    }
};