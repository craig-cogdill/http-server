#include "HttpServerTest.h"
#include "MockHttpServer.h"

TEST_F(HttpServerTest, SanityCheck) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_TRUE(nullptr != mockHttpServer);

}