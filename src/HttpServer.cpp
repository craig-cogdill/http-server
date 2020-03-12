#include "HttpServer.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

std::unique_ptr<HttpServer> HttpServer::Create() {
    std::unique_ptr<HttpServer> httpServerPtr(new HttpServer);
    if (!httpServerPtr->InitializeSocket()) {
        return nullptr;
    }
    return httpServerPtr;
}

HttpServer::HttpServer():
    mSocketFd(-1),
    mSocketAddr{},
    kPortNumber(8000),
    mCRLF("\r\n") {
}

HttpServer::~HttpServer() { 
    if (mSocketFd > 0) {
        close(mSocketFd);
    }
}

bool HttpServer::InitializeSocket() {
    if ((mSocketFd = Socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to initialize socket" << std::endl;
        return false;
    }
    
    mSocketAddr.sin_family = AF_INET;
    mSocketAddr.sin_addr.s_addr = INADDR_ANY;
    mSocketAddr.sin_port = htons(kPortNumber);
    memset(mSocketAddr.sin_zero, '\0', sizeof mSocketAddr.sin_zero);
    
    if (Bind(mSocketFd, (struct sockaddr *) &mSocketAddr, sizeof(mSocketAddr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    if (Listen(mSocketFd, 10) < 0) {
        std::cerr << "Failed to set socket listen" << std::endl;
        return false;
    }
    // Set the socket to non-binding mode
    if (Fcntl(mSocketFd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Failed to set socket non-binding" << std::endl;
        return false;
    }
    return true;
}

void HttpServer::ProcessRequest() {
    char buffer[30000] = {0};
    long bytesRead(0);
    socklen_t socketAddrLen = sizeof(mSocketAddr);
    int openConnection = Accept(mSocketFd, reinterpret_cast<sockaddr*>(&mSocketAddr), &socketAddrLen);

    // errno will be set to EAGAIN or EWOULDBLOCK if there are no pending
    //      socket connections. Both must be checked for portability.
    if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        if (openConnection < 0) {
            std::cerr << "Error reading from socket: " << openConnection << std::endl;
            Close(openConnection);
            exit(EXIT_FAILURE);
        } else {
            bytesRead = Read(openConnection, buffer, 30000);
            if (0 == bytesRead) {
                std::cerr << "HTTP Server received an unexpectedly empty message." << std::endl;
            } else {
                HttpRequest request(&buffer[0]);
                std::string response = HandleRequestAndGenerateResponse(request);
                ssize_t bytesWritten = Write(openConnection, response.c_str(), strlen(response.c_str()));
                if (bytesWritten <= 0) {
                    std::cerr << "Failed to send response to client" << std::endl;
                }
            }
        }
    }
    Close(openConnection);
}

std::string HttpServer::HandleRequestAndGenerateResponse(HttpRequest& request) {
    std::string response("");
    if (!request.IsValid()) {
        response = GetResponseFromError(request.GetBadRequestReturnCode());
    } else {
        if ("GET" == request.GetVerb()) { 
            response = HandleGetRequest(request.GetUri());
        } else if ("POST" == request.GetVerb()) {
            response = HandlePostRequest(request);
        } else if ("DELETE" == request.GetVerb()) {
            response = HandleDeleteRequest(request.GetUri());
        }
    }
    return response;
}

std::string HttpServer::HandleGetRequest(const std::string& uri) {
    std::string response("");
    if (mDatabase.find(uri) == mDatabase.end()) {
        response = GetResponseFromError(404);
    } else {
        // This call would usually be wrapped in a try{}catch{} since it throws.
        //    However, this server handles requests sequentially on a single socket,
        //    at this point, it has been confirmed the data exists in the table.
        //    It is impossible for it to be overwritten before this call, so this is safe. 
        DataTuple dataTuple = mDatabase.at(uri);

        // This is the only point in the server where headers and data
        //   need to be returned, so it is built into this function,
        //   rather than being in it's own function
        response += "HTTP/1.1 200 OK"+mCRLF;
        std::string contentType = std::get<0>(dataTuple);
        std::string contentLength = std::get<1>(dataTuple);
        std::string data = std::get<2>(dataTuple);
        if (!contentType.empty()) {
            response += "Content-Type: "+contentType+mCRLF;
        }
        if (!contentLength.empty()) {
            response += "Content-Length: "+contentLength+mCRLF;
        }
        response += mCRLF+mCRLF;
        response += data+mCRLF;
    }
    return response;    
}

std::string HttpServer::HandleDeleteRequest(const std::string& uri) {
    std::string response("");
    auto dbEntryItr = mDatabase.find(uri);
    if (mDatabase.end() == dbEntryItr) {
        response = GetResponseFromError(404);
    } else {
        mDatabase.erase(dbEntryItr);
        response = "HTTP/1.1 200 OK"+mCRLF;
    }
    return response;    
}
    
std::string HttpServer::HandlePostRequest(HttpRequest& request) {
    DataTuple dataTuple = std::make_tuple(request.GetContentType(), request.GetContentLength(), request.GetData());
    mDatabase[request.GetUri()] = dataTuple;
    return std::string("HTTP/1.1 200 OK"+mCRLF);
}
    
std::string HttpServer::GetResponseFromError(int errorCode) {
    std::string responseString("HTTP/1.1 ");
    switch (errorCode) {
    case 400:
        responseString += "400 Bad Request";
        break;
    case 404:
        responseString += "404 Not Found";
        break;
    case 405:
        responseString += "405 Method Not Allowed";
        break;
    default:
        responseString += "500 Internal Server Error";
        break;
    }
    return responseString+mCRLF;
}
    
// System call wrappers

int HttpServer::Socket(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
}
    
int HttpServer::Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    return bind(sockfd, addr, addrlen);
}
    
int HttpServer::Listen(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}
    
int HttpServer::Fcntl(int fd, int cmd, int val) {
    return fcntl(fd, cmd, val);
}
    
int HttpServer::Accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    return accept(sockfd, addr, addrlen);
}

ssize_t HttpServer::Read(int fd, void* buf, size_t count) {
    return read(fd, buf, count);
}
    
ssize_t HttpServer::Write(int fd, const void* buf, size_t count) {
    return write(fd, buf, count);
}
    
int HttpServer::Close(int fildes) {
    return close(fildes);
}    