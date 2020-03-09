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
    virtual bool InitializeSocket();

    // System call wrappers
    virtual int Socket(int domain, int type, int protocol);
    virtual int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    virtual int Listen(int sockfd, int backlog);
    virtual int Fcntl(int fd, int cmd, int val);

private:
    int mSocketFd;
    struct sockaddr_in mSocketAddr;
    uint16_t kPortNumber;
};