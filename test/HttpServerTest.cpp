#include "HttpServerTest.h"
#include "MockHttpServer.h"
#include "MockHttpRequest.h"

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
    HttpRequest request = mockHttpServer->ReadFromSocket();
    EXPECT_FALSE(request.IsValid());
    EXPECT_TRUE(request.GetVerb().empty());
}

TEST_F(HttpServerTest, ReadFromSocket_AcceptHasNoConnections_EWOULDBLOCK) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mErrnoRetVal = EWOULDBLOCK;
    HttpRequest request = mockHttpServer->ReadFromSocket();
    EXPECT_FALSE(request.IsValid());
    EXPECT_TRUE(request.GetVerb().empty());
}

TEST_F(HttpServerTest, ReadFromSocket_ReadNothing) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    mockHttpServer->mReadRetVal = 0;
    HttpRequest request = mockHttpServer->ReadFromSocket();
    EXPECT_FALSE(request.IsValid());
    EXPECT_TRUE(request.GetVerb().empty());
}

TEST_F(HttpServerTest, ReadFromSocket_ReadValidRequest) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    // Mock out that the call to 'read' returns a valid request
    std::string validRequest("GET /tmp/uri HTTP/1.1\r\n");
    mockHttpServer->mSetBufferInRead = true;
    mockHttpServer->mReadBufferRetVal = validRequest; 
    mockHttpServer->mReadRetVal = validRequest.size();

    HttpRequest request = mockHttpServer->ReadFromSocket();
    EXPECT_TRUE(request.IsValid());
    EXPECT_EQ("GET", request.GetVerb());
}

TEST_F(HttpServerTest, GetResponseFromError_400) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    int err400(400);
    std::string expectedError("HTTP/1.1 400 Bad Request"+gCRLF);

    EXPECT_EQ(expectedError, mockHttpServer->GetResponseFromError(err400));
}

TEST_F(HttpServerTest, GetResponseFromError_404) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    int err404(404);
    std::string expectedError("HTTP/1.1 404 Not Found"+gCRLF);

    EXPECT_EQ(expectedError, mockHttpServer->GetResponseFromError(err404));
}

TEST_F(HttpServerTest, GetResponseFromError_405) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    int err405(405);
    std::string expectedError("HTTP/1.1 405 Method Not Allowed"+gCRLF);

    EXPECT_EQ(expectedError, mockHttpServer->GetResponseFromError(err405));
}

TEST_F(HttpServerTest, GetResponseFromError_999) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    int err999(999);
    std::string expectedError("HTTP/1.1 500 Internal Server Error"+gCRLF);

    EXPECT_EQ(expectedError, mockHttpServer->GetResponseFromError(err999));
}

TEST_F(HttpServerTest, GetResponseStringFromRequest_Error400) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    MockHttpRequest mockHttpRequest;
    mockHttpRequest.SetValid(false);
    mockHttpRequest.SetBadRequestReturnCode(gBadRequest);

    std::string expectedBadRequestResponseString("HTTP/1.1 400 Bad Request"+gCRLF);
    EXPECT_EQ(expectedBadRequestResponseString, mockHttpServer->HandleRequest(mockHttpRequest));
}

TEST_F(HttpServerTest, HandleGET_DoesNotExist) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    std::string uri("/test/uri");
    std::string doesNotExistError("HTTP/1.1 404 Not Found"+gCRLF);

    EXPECT_EQ(doesNotExistError, mockHttpServer->HandleGetRequest(uri));
}

TEST_F(HttpServerTest, HandleGET_ReturnValidData) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    // build test data
    std::string uri("/test/uri");
    std::string contentType("text/plain");
    std::string data("Hello World!");
    std::string contentLength(std::to_string(data.length()));
    mockHttpServer->AddEntryToDatabase(uri, contentType, contentLength, data);
    size_t expectedDbSize(1);
    EXPECT_EQ(expectedDbSize, mockHttpServer->GetDatabaseSize());

    // build expected response
    std::string goodResponseHeader("HTTP/1.1 200 OK"+gCRLF);
    std::string headers("Content-Type: "+contentType+gCRLF+"Content-Length: "+contentLength+gCRLF);
    std::string expectedResponse(goodResponseHeader+headers+gCRLF+gCRLF+data+gCRLF);

    EXPECT_EQ(expectedResponse, mockHttpServer->HandleGetRequest(uri));
}

TEST_F(HttpServerTest, HandleGET_ReturnValidDataWithNoHeaders) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    // build test data
    std::string uri("/test/uri");
    std::string data("Hello World!");
    mockHttpServer->AddEntryToDatabase(uri, "", "", data);
    size_t expectedDbSize(1);
    EXPECT_EQ(expectedDbSize, mockHttpServer->GetDatabaseSize());

    // build expected response
    std::string goodResponseHeader("HTTP/1.1 200 OK"+gCRLF);
    std::string expectedResponse(goodResponseHeader+gCRLF+gCRLF+data+gCRLF);

    EXPECT_EQ(expectedResponse, mockHttpServer->HandleGetRequest(uri));
}

TEST_F(HttpServerTest, HandleDELETE_DoesNotExist) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    std::string uri("/test/uri");
    std::string doesNotExistError("HTTP/1.1 404 Not Found"+gCRLF);

    EXPECT_EQ(doesNotExistError, mockHttpServer->HandleDeleteRequest(uri));
}

TEST_F(HttpServerTest, HandleDELETE_ValidDataDeleted) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    // build test data
    std::string uri("/test/uri");
    std::string contentType("text/plain");
    std::string data("Hello World!");
    std::string contentLength(std::to_string(data.length()));
    mockHttpServer->AddEntryToDatabase(uri, contentType, contentLength, data);
    size_t expectedDbSize(1);
    EXPECT_EQ(expectedDbSize, mockHttpServer->GetDatabaseSize());

    // build expected response
    std::string expectedResponse("HTTP/1.1 200 OK"+gCRLF);

    EXPECT_EQ(expectedResponse, mockHttpServer->HandleDeleteRequest(uri));

    // Expect that the database is empty
    size_t expectedEmptyDbSize(0);
    EXPECT_EQ(expectedEmptyDbSize, mockHttpServer->GetDatabaseSize());
}

TEST_F(HttpServerTest, HandlePOST_EntryAdded) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);


    // database is empty before POST
    size_t emptyDbSize(0);
    EXPECT_EQ(emptyDbSize, mockHttpServer->GetDatabaseSize());

    // build test data
    std::string uri("/test/uri");
    std::string contentType("text/plain");
    std::string data("Hello World!");
    std::string contentLength(std::to_string(data.length()));
    MockHttpRequest mockHttpRequest;
    mockHttpRequest.SetVerb("POST");
    mockHttpRequest.SetUri(uri);
    mockHttpRequest.SetContentType(contentType);
    mockHttpRequest.SetContentLength(contentLength);
    mockHttpRequest.SetData(data);

    std::string expectedResponse("HTTP/1.1 200 OK"+gCRLF);
    EXPECT_EQ(expectedResponse, mockHttpServer->HandlePostRequest(mockHttpRequest));

    // one entry in the database
    size_t expectedDbSize(1);
    EXPECT_EQ(expectedDbSize, mockHttpServer->GetDatabaseSize());
}

TEST_F(HttpServerTest, HandlePOST_EntryOverwritten) {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    EXPECT_NE(nullptr, mockHttpServer);

    // build test data
    std::string uri("/test/uri");
    std::string contentType("text/plain");
    std::string data("Hello World!");
    std::string contentLength(std::to_string(data.length()));
    MockHttpRequest mockHttpRequest;
    mockHttpRequest.SetVerb("POST");
    mockHttpRequest.SetUri(uri);
    mockHttpRequest.SetContentType(contentType);
    mockHttpRequest.SetContentLength(contentLength);
    mockHttpRequest.SetData(data);

    // build expected response
    std::string expectedResponse("HTTP/1.1 200 OK"+gCRLF);
    EXPECT_EQ(expectedResponse, mockHttpServer->HandlePostRequest(mockHttpRequest));
    size_t expectedDbSize(1);
    EXPECT_EQ(expectedDbSize, mockHttpServer->GetDatabaseSize());

    // change the request data
    data = "This is some new data to overwrite Hello World!";
    mockHttpRequest.SetData(data);
    EXPECT_EQ(expectedResponse, mockHttpServer->HandlePostRequest(mockHttpRequest));
    EXPECT_EQ(expectedDbSize, mockHttpServer->GetDatabaseSize());
    EXPECT_EQ(data, mockHttpServer->GetDataFromDatabase(uri));
}