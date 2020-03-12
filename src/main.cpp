#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "HttpServer.h"

namespace {
   volatile sig_atomic_t gContinueRunning(0);
}

void shutdownSignalHandler(int signalCode) {
   gContinueRunning = signalCode;
}

int main(int argc, char *argv[]) {
   signal(SIGTERM, shutdownSignalHandler);
   signal(SIGINT, shutdownSignalHandler);

   std::unique_ptr<HttpServer> requestServerPtr = HttpServer::Create();
   if (nullptr == requestServerPtr) {
      std::cerr << "Failed to intialize HTTP Server. Cannot continue processing." << std::endl;
      return -1;
   }

   while (0 == gContinueRunning) {
      requestServerPtr->ProcessRequest();

      // A small sleep will be a significant CPU saver when no
      //   connection is present on the socket, since the call
      //   to accept new connections is non-blocking
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }

   return 0;
}
