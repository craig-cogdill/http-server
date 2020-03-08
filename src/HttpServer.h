#pragma once
#include <string>

class HttpServer {
public:
    HttpServer(): kPortNumber(8000) {};
    virtual ~HttpServer() = default;

    std::string what();
    bool Setup();
protected:
private:
    uint16_t kPortNumber;
};