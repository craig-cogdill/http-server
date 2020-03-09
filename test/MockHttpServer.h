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
    virtual ~MockHttpServer() {}

    bool InitializeSocket() override {
        return HttpServer::InitializeSocket();
    }

    int Socket(int domain, int type, int protocol) override {
        return mSocketRetVal;
    }
    
    int Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) override {
        return mBindRetVal;
    }
    
    int Listen(int sockfd, int backlog) override {
        return mListenRetVal;
    }
    
    int Fcntl(int fd, int cmd, int val) override {
        return mFcntlRetVal;
    }
 
    int Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) override {
        errno = mErrnoRetVal;
        return mAcceptRetVal;
    }
    
    ssize_t Read(int fd, void* buf, size_t count) override {
        return mReadRetVal;
    }

    // Return variables for targeting mocking of system calls
    //    All variables defaulted to 'happy path' return values
    //
    // NOTE: For the socket() return val, a negative integer is passed,
    //       as passing a positive integer back in the mock will cause
    //       an errant call to close() in the destructor
    int mSocketRetVal{-1};
    int mBindRetVal{0};
    int mListenRetVal{0};
    int mFcntlRetVal{0};
    int mAcceptRetVal{0};
    ssize_t mReadRetVal{1};

    int mErrnoRetVal{0};
};