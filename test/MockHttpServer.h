#include "HttpServer.h"

class MockHttpSever : public HttpServer {
    public:
        MockHttpSever() : HttpServer() {}
        virtual ~MockHttpSever() = default;
    protected:
    private:
};