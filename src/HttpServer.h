#pragma once
#include <string>
#include <memory>
#include <unistd.h>
#include <netinet/in.h>

class HttpServer {
public:
    static std::unique_ptr<HttpServer> CreateHttpServer();
    virtual ~HttpServer() { close(mSocketFd); }

    std::string what();
    std::string ReadFromSocket();
protected:
    HttpServer(): mSocketFd(-1), mSocketAddr{}, kPortNumber(8000) {};
    bool Setup();
private:
    int mSocketFd;
    struct sockaddr_in mSocketAddr;
    uint16_t kPortNumber;
};