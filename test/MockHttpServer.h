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
protected:
private:
};