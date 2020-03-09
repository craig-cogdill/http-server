#include "HttpServerTest.h"
#include "MockHttpServer.h"

TEST_F(HttpServerTest, BasicInstantiationOfMockServer) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);
}

TEST_F(HttpServerTest, InitializeSocket_HappyPath) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    // All mocked return values are 'happy path' by default
    EXPECT_TRUE(mockHttpServer->InitializeSocket());
}