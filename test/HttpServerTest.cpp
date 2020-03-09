#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "MockHttpServer.h"

TEST_CASE("Basic instantiation") {
    std::unique_ptr<MockHttpServer> mockHttpServer = MockHttpServer::Create();
    REQUIRE(nullptr != mockHttpServer);

    SECTION("Default section") {
        REQUIRE(true);
    }
}