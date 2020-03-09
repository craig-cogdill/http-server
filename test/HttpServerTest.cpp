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

TEST_F(HttpServerTest, InitializeSocket_SocketFailed) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mSocketRetVal = 0;
    EXPECT_FALSE(mockHttpServer->InitializeSocket());    
}

TEST_F(HttpServerTest, InitializeSocket_BindFailed) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mBindRetVal = -1;
    EXPECT_FALSE(mockHttpServer->InitializeSocket());    
}

TEST_F(HttpServerTest, InitializeSocket_ListenFailed) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mListenRetVal = -1;
    EXPECT_FALSE(mockHttpServer->InitializeSocket());    
}

TEST_F(HttpServerTest, InitializeSocket_FcntlFailed) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mFcntlRetVal = -1;
    EXPECT_FALSE(mockHttpServer->InitializeSocket());    
}

TEST_F(HttpServerTest, ReadFromSocket_AcceptHasNoConnections_EAGAIN) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mErrnoRetVal = EAGAIN;
    EXPECT_EQ("", mockHttpServer->ReadFromSocket());    
}

TEST_F(HttpServerTest, ReadFromSocket_AcceptHasNoConnections_EWOULDBLOCK) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mErrnoRetVal = EWOULDBLOCK;
    EXPECT_EQ("", mockHttpServer->ReadFromSocket());    
}

TEST_F(HttpServerTest, ReadFromSocket_ReadNothing) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mReadRetVal = 0;
    EXPECT_EQ("", mockHttpServer->ReadFromSocket());    
}