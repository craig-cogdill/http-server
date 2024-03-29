#pragma once
#include <string>
#include <memory>
#include <netinet/in.h>
#include <unordered_map>
#include "HttpRequest.h"

typedef std::tuple<std::string, std::string, std::string> DataTuple;

class HttpServer {
public:
    // A factory function is the only mechanism for a user to instantiate an object
    //   of this class. 
    //
    // InitializeSocket must always be called after object construction. To ensure
    //   that the user does not forget to call it, it will be called in the factory
    //   function after object construction. Should InitializeSocket fail, a nullptr
    //   will be returned to the caller.
    static std::unique_ptr<HttpServer> Create();
    virtual ~HttpServer();

    void ProcessRequest();

protected:
    // This constructor is protected per the factory pattern
    HttpServer();

    bool InitializeSocket();
    std::string HandleRequestAndGenerateResponse(HttpRequest& request);
    std::string HandleGetRequest(const std::string& uri);
    std::string HandleDeleteRequest(const std::string& uri);
    std::string HandlePostRequest(HttpRequest& request);
    std::string GetResponseFromError(int errorCode);

    // System call wrappers
    virtual int Socket(int domain, int type, int protocol);
    virtual int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    virtual int Listen(int sockfd, int backlog);
    virtual int Fcntl(int fd, int cmd, int val);
    virtual int Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    virtual ssize_t Read(int fd, void* buf, size_t count);
    virtual ssize_t Write(int fd, const void* buf, size_t count);
    virtual int Close(int fildes);

    std::unordered_map<std::string, DataTuple> mDatabase;

private:
    int mSocketFd;
    struct sockaddr_in mSocketAddr;
    uint16_t kPortNumber;
    const std::string mCRLF;
    int kMaxBufferSize;
};