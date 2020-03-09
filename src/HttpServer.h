#pragma once
#include <string>
#include <memory>
#include <unistd.h>
#include <netinet/in.h>

class HttpServer {
public:
    static std::unique_ptr<HttpServer> Create();
    virtual ~HttpServer() { close(mSocketFd); }

    std::string ReadFromSocket();

protected:
    HttpServer(): mSocketFd(-1), mSocketAddr{}, kPortNumber(8000) {};
    bool InitializeSocket();

    // System call wrappers
    int Socket(int domain, int type, int protocol);
    int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int Listen(int sockfd, int backlog);

private:
    int mSocketFd;
    struct sockaddr_in mSocketAddr;
    uint16_t kPortNumber;
};