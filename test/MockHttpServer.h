#include "HttpServer.h"

class MockHttpServer : public HttpServer {
    public:
        MockHttpServer() : HttpServer() {}
        virtual ~MockHttpServer() = default;
    protected:
    private:
};