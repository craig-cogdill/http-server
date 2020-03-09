
#include "gtest/gtest.h"

class HttpRequestTest : public ::testing::Test {
public:
    HttpRequestTest(): mEmptyRequest("") {};
    virtual ~HttpRequestTest() = default;
    std::string mEmptyRequest;
};