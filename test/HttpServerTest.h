#include "gtest/gtest.h"

class HttpServerTest : public ::testing::Test {
public:
    HttpServerTest(): 
        gBadRequest(400),
        gVerbNotAllowed(405),
        gCRLF("\r\n") {
    }

    virtual ~HttpServerTest() = default;
    
    int gBadRequest;
    int gVerbNotAllowed;
    const std::string gCRLF;
};