#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "HttpServer.h"

void shutdownSignalHandler(int signalCode) {
   std::cout << "Interrupt signal caught: " << signalCode << std::endl;
   exit(signalCode);
}

int main(int argc, char *argv[]) {
   signal(SIGTERM, shutdownSignalHandler);
   signal(SIGINT, shutdownSignalHandler);

   HttpServer requestServer;
   if (!requestServer.Setup()) {
      std::cerr << "Failed to setup socket, cannot continue" << std::endl;
      exit(-1);
   }

   while (true) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      std::cout << requestServer.what() << std::endl;
   }

   return 0;
}
