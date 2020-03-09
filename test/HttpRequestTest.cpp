#include "HttpRequestTest.h"
#include "HttpRequest.h"

TEST_F(HttpRequestTest, Instantiation) {
    std::string badRequest{"Malformed Request"};
    HttpRequest httpRequest(badRequest);
}

TEST_F(HttpRequestTest, Explode_emptyString) {
    std::string emptyString{""};
    char delim{'\n'};

    HttpRequest httpRequest(emptyString);
    std::vector<std::string> explodedStr = httpRequest.Explode(emptyString, delim);
    EXPECT_TRUE(explodedStr.empty());
}

TEST_F(HttpRequestTest, Explode_MultiLineString) {
    std::string firstElement{"hello"};
    std::string secondElement{"world"};
    std::string testString{firstElement+"\n"+secondElement};
    char delim{'\n'};

    HttpRequest httpRequest(testString);
    std::vector<std::string> explodedStr = httpRequest.Explode(testString, delim);
    
    EXPECT_FALSE(explodedStr.empty());
    EXPECT_TRUE(2 == explodedStr.size());
    EXPECT_EQ(firstElement, explodedStr.at(0));
    EXPECT_EQ(secondElement, explodedStr.at(1));
}

TEST_F(HttpRequestTest, Explode_MultipleEmptyLines) {
    std::string firstElement{"hello"};
    std::string secondElement{"world"};
    std::string thirdElement{"test"};
    std::string testString{firstElement+"\n"+secondElement+"\n\n"+thirdElement};
    char delim{'\n'};

    HttpRequest httpRequest(testString);
    std::vector<std::string> explodedStr = httpRequest.Explode(testString, delim);
    
    EXPECT_FALSE(explodedStr.empty());
    ASSERT_TRUE(4 == explodedStr.size());
    EXPECT_EQ(firstElement, explodedStr.at(0));
    EXPECT_EQ(secondElement, explodedStr.at(1));
    EXPECT_EQ(thirdElement, explodedStr.at(3));
}

TEST_F(HttpRequestTest, Explode_SpaceAsDelimiter) {
    std::string firstElement{"hello"};
    std::string secondElement{"world"};
    std::string testString{firstElement+" "+secondElement};
    char delim{' '};

    HttpRequest httpRequest(testString);
    std::vector<std::string> explodedStr = httpRequest.Explode(testString, delim);
    
    EXPECT_FALSE(explodedStr.empty());
    ASSERT_TRUE(2 == explodedStr.size());
    EXPECT_EQ(firstElement, explodedStr.at(0));
    EXPECT_EQ(secondElement, explodedStr.at(1));
}

TEST_F(HttpRequestTest, Explode_SemiColonAsDelimiter) {
    std::string firstElement{"User-Agent"};
    std::string secondElement{" curl/7.35.0"};
    std::string testString{firstElement+":"+secondElement};
    char delim{':'};

    HttpRequest httpRequest(testString);
    std::vector<std::string> explodedStr = httpRequest.Explode(testString, delim);
    
    EXPECT_FALSE(explodedStr.empty());
    ASSERT_TRUE(2 == explodedStr.size());
    EXPECT_EQ(firstElement, explodedStr.at(0));
    EXPECT_EQ(secondElement, explodedStr.at(1));
}

TEST_F(HttpRequestTest, ParseFirstLine_EmptyRequest) {
    std::string emptyRequest{""};
    
    HttpRequest httpRequest(emptyRequest);
    EXPECT_FALSE(httpRequest.ParseFirstLine(emptyRequest));
}

TEST_F(HttpRequestTest, ParseFirstLine_NotEnoughFirstLineArgs_1) {
    std::string request{"GET /fake/uri"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_NotEnoughFirstLineArgs_2) {
    std::string request{"GET /fake/uri"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_BadVerb1) {
    std::string request{"PUT /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_BadVerb2) {
    std::string request{"HEAD /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_BadHttpVersion) {
    std::string request{"HEAD /fake/uri HTTP/10.11"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_Valid_GET) {
    std::string request{"GET /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_TRUE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_Valid_POST) {
    std::string request{"POST /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_TRUE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseFirstLine_Valid_DELETE) {
    std::string request{"DELETE /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_TRUE(httpRequest.ParseFirstLine(request));
}

TEST_F(HttpRequestTest, ParseHeaders_NoHeaders) {
    std::vector<std::string> emptyHeaders{};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.ParseHeaders(emptyHeaders));
}

TEST_F(HttpRequestTest, ParseHeaders_MalformedHeader) {
    std::string malformedHeader1{"thisHeaderIsMalformed"};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.ParseHeaders({malformedHeader1}));
}

TEST_F(HttpRequestTest, ParseHeaders_ContentType) {
    std::string contentType{"application/json"};
    std::string header1{"Content-Type: "+contentType};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.ParseHeaders({header1}));

    EXPECT_EQ(contentType, httpRequest.mContentType);
}







TEST_F(HttpRequestTest, Parse_EmtpyRequest) {
    std::string emptyRequest{""};
    
    HttpRequest httpRequest(emptyRequest);
    EXPECT_FALSE(httpRequest.Parse(emptyRequest));
}

TEST_F(HttpRequestTest, Parse_BadVerb) {
    std::string request{"PUT /uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.Parse(request));
}

TEST_F(HttpRequestTest, Parse_BadHttpVersion) {
    std::string request{"PUT /uri HTTP/100"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.Parse(request));
}

TEST_F(HttpRequestTest, Parse_Valid) {
    std::string request{"GET /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_TRUE(httpRequest.Parse(request));
}