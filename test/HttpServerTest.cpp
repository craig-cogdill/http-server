#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "MockHttpServer.h"

TEST_CASE("Basic instantiation") {
    MockHttpServer mockHttpServer;

    SECTION("Default section") {
        REQUIRE(true);
    }
}