#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include "HttpServer.h"
#include "HttpRequest.h"

namespace {
   volatile sig_atomic_t gContinueRunning(0);
   int gExitCode(0);
}

void shutdownSignalHandler(int signalCode) {
   gContinueRunning = signalCode;
   gExitCode = signalCode;
}

int main(int argc, char *argv[]) {
   signal(SIGTERM, shutdownSignalHandler);
   signal(SIGINT, shutdownSignalHandler);

   std::unique_ptr<HttpServer> requestServerPtr = HttpServer::Create();
   if (nullptr == requestServerPtr) {
      std::cerr << "Failed to intialize HTTP Server. Cannot continue processing." << std::endl;
      exit(-1);
   }

   while (0 == gContinueRunning) {
      HttpRequest request = requestServerPtr->ReadFromSocket();
      if (!request.IsEmpty()) {
         std::string response = requestServerPtr->HandleRequest(request);
         std::cout << "==============================" << std::endl;
         std::cout << response << std::endl;
         std::cout << "==============================" << std::endl;
      }

      // A small sleep will be a significant CPU saver when no
      //   connection is present on the socket
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }

   return gExitCode;
}
