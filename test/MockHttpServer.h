#include "HttpServer.h"

/*
    This class exists to test the HttpServer class member functions.
    For the sake of unit tests, an actual socket should not be created.
    To avoid this, the HttpServer::Setup() function is not called as part
    of MockHttpServer instantiation. It will be explicitly called in tests
    after appropriate steps to mock system calls are taken.
*/
class MockHttpServer : public HttpServer {
public:
    static std::unique_ptr<MockHttpServer> Create() {
       return std::unique_ptr<MockHttpServer>(new MockHttpServer);
    }
    MockHttpServer() : HttpServer() {}
    virtual ~MockHttpServer() = default;

    bool InitializeSocket() {
        return HttpServer::InitializeSocket();
    }
    int Socket(int domain, int type, int protocol) {
        return 1;
    }
    
    int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
        return 0;
    }
    
    int Listen(int sockfd, int backlog) {
        return 0;
    }
    
    int Fcntl(int fd, int cmd, int val) {
        return 0;
    }
protected:
private:
};