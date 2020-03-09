#include "HttpServer.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <cerrno>
#include <fcntl.h>

std::unique_ptr<HttpServer> HttpServer::CreateHttpServer() {
    std::unique_ptr<HttpServer> httpServerPtr(new HttpServer);
    if (!httpServerPtr->Setup()) {
        return nullptr;
    }
    return httpServerPtr;
}

std::string HttpServer::what() {
    return std::string{"HttpServer"};
}

bool HttpServer::Setup() {
    // Creating socket file descriptor
    if ((mSocketFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to initialize socket" << std::endl;
        return false;
    }
    
    mSocketAddr.sin_family = AF_INET;
    mSocketAddr.sin_addr.s_addr = INADDR_ANY;
    mSocketAddr.sin_port = htons(kPortNumber);
    memset(mSocketAddr.sin_zero, '\0', sizeof mSocketAddr.sin_zero);
    
    
    if (bind(mSocketFd, (struct sockaddr *) &mSocketAddr, sizeof(mSocketAddr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    if (listen(mSocketFd, 10) < 0) {
        std::cerr << "Failed to set socket listen" << std::endl;
        return false;
    }
    // Set the socket to non-binding mode
    fcntl(mSocketFd, F_SETFL, O_NONBLOCK);
    return true;
}

std::string HttpServer::ReadFromSocket() {
    std::string toReturn{""};

    int socketAddrLen = sizeof(mSocketAddr);
    int newSocket = accept(mSocketFd, (struct sockaddr *) &mSocketAddr, (socklen_t*) &socketAddrLen);

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
        
    //write(new_socket , hello , strlen(hello));
    //printf("------------------Hello message sent-------------------\n");
}