#include "HttpServer.h"

/*
    This class exists to help test the HttpServer class member functions.
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

    bool InitializeSocket() {
        return HttpServer::InitializeSocket();
    }

    std::string HandleRequestAndGenerateResponse(HttpRequest& request) {
        return HttpServer::HandleRequestAndGenerateResponse(request);
    }

    std::string HandleGetRequest(const std::string& uri) {
        return HttpServer::HandleGetRequest(uri);
    }
    
    std::string HandleDeleteRequest(const std::string& uri) {
        return HttpServer::HandleDeleteRequest(uri);
    }
    
    std::string HandlePostRequest(HttpRequest& request) {
        return HttpServer::HandlePostRequest(request);
    }

    std::string GetResponseFromError(int errorCode) {
        return HttpServer::GetResponseFromError(errorCode);
    }

    
    // Mock-specific functions

    void AddEntryToDatabase(const std::string& uri, const std::string& contentType,
        const std::string& contentLength, const std::string& data) {
        DataTuple dataTuple = std::make_tuple(contentType, contentLength, data);

        mDatabase[uri] = dataTuple;
    }

    std::string GetDataFromDatabase(const std::string& uri) {
        std::string data("");
        if (mDatabase.find(uri) != mDatabase.end()) {
            DataTuple dataTuple = mDatabase.at(uri);
            data = std::get<2>(dataTuple);
        }
        return data;
    }

    size_t GetDatabaseSize() {
        return mDatabase.size();
    }

    // System call overrides

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
        if (mSetBufferInRead) {
            strcpy(static_cast<char*>(buf), mReadBufferRetVal.c_str());
        }
        return mReadRetVal;
    }

    ssize_t Write(int fd, const void* buf, size_t count) override {
        return count;
    }

    int Close(int fildes) override {
        return mCloseRetVal;
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
    int mCloseRetVal{0};

    int mErrnoRetVal{0};
    bool mSetBufferInRead{false};
    std::string mReadBufferRetVal{""};
};