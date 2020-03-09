#pragma once
#include <string>
#include <memory>
#include <unistd.h>
#include <netinet/in.h>

class HttpServer {
public:
    static std::unique_ptr<HttpServer> CreateHttpServer();
    virtual ~HttpServer() { close(mSocket); }

    std::string what();
    std::string ReadFromSocket();
protected:
    HttpServer(): mSocketFd(-1), mSocket(-1), mSocketAddr{}, kPortNumber(8000) {};
    bool Setup();
private:
    int mSocketFd;
    int mSocket;
    struct sockaddr_in mSocketAddr;
    uint16_t kPortNumber;
};