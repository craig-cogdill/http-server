#include "HttpServer.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

std::string HttpServer::what() {
    return std::string{"HttpServer"};
}

bool HttpServer::Setup() {
    int server_fd(-1);
    //int new_socket(-1);
    //long valread;

    struct sockaddr_in address;
    //int addrlen = sizeof(address);
    
    //std::string helloStr("Hello from server");
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to initialize socket" << std::endl;
        return false;
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(kPortNumber);
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to set socket listen" << std::endl;
        return false;
    }
    return true;
}