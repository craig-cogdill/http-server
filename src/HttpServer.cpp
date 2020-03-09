#include "HttpServer.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <cerrno>
#include <fcntl.h>

std::unique_ptr<HttpServer> HttpServer::Create() {
    std::unique_ptr<HttpServer> httpServerPtr(new HttpServer);
    if (!httpServerPtr->InitializeSocket()) {
        return nullptr;
    }
    return httpServerPtr;
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

std::string HttpServer::ReadFromSocket() {
    std::string toReturn{""};

    socklen_t socketAddrLen = sizeof(mSocketAddr);
    int newSocket = accept(mSocketFd, reinterpret_cast<sockaddr*>(&mSocketAddr), &socketAddrLen);

    // errno will be set to EAGAIN or EWOULDBLOCK if there are no pending
    //      socket connections. Both must be checked for portability.
    if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        if (newSocket < 0) {
            std::cerr << "Error reading from socket: " << newSocket << std::endl;
            close(newSocket);
            exit(EXIT_FAILURE);
        } else {
            char buffer[30000] = {0};
            long valread = read(newSocket, buffer, 30000);
            if (valread > 0) {
                toReturn = std::string(&buffer[0], valread);
            }
        }
    }
    close(newSocket);
    return toReturn;
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