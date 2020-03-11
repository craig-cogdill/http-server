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
      if (!request.GetVerb().empty()) {
         std::cout << "====================================" << std::endl;
         //std::cout << message << std::endl;
         //std::cout << "====================================" << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }

   return gExitCode;
}
