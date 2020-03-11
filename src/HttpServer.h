#pragma once
#include <string>
#include <memory>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <unordered_map>
#include "HttpRequest.h"

typedef std::tuple<std::string, std::string, std::string> DataTuple;

class HttpServer {
public:
    static std::unique_ptr<HttpServer> Create();
    virtual ~HttpServer();

    HttpRequest ReadFromSocket();
    std::string HandleRequest(HttpRequest& request);

protected:
    HttpServer():
        mSocketFd(-1),
        mSocketAddr{},
        kPortNumber(8000),
        mCRLF("\r\n") {
    };

    virtual bool InitializeSocket();
    virtual std::string GetResponseFromError(int errorCode);
    virtual std::string HandleGetRequest(const std::string& uri);
    virtual std::string HandleDeleteRequest(const std::string& uri);
    virtual std::string HandlePostRequest(HttpRequest& request);

    // System call wrappers
    virtual int Socket(int domain, int type, int protocol);
    virtual int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    virtual int Listen(int sockfd, int backlog);
    virtual int Fcntl(int fd, int cmd, int val);
    virtual int Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    virtual ssize_t Read(int fd, void* buf, size_t count);

    std::unordered_map<std::string, DataTuple> mDatabase;

private:
    int mSocketFd;
    struct sockaddr_in mSocketAddr;
    uint16_t kPortNumber;
    const std::string mCRLF;
};