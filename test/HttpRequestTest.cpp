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
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.ParseFirstLine(mEmptyRequest));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, ParseFirstLine_NotEnoughFirstLineArgs_1) {
    std::string request{"GET /fake/uri"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, ParseFirstLine_NotEnoughFirstLineArgs_2) {
    std::string request{"GET /fake/uri"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, ParseFirstLine_BadVerb1) {
    std::string request{"PUT /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
    EXPECT_EQ(405, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, ParseFirstLine_BadVerb2) {
    std::string request{"HEAD /fake/uri HTTP/1.1"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
    EXPECT_EQ(405, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, ParseFirstLine_BadHttpVersion) {
    std::string request{"GET /fake/uri HTTP/10.11"};
    
    HttpRequest httpRequest(request);
    EXPECT_FALSE(httpRequest.ParseFirstLine(request));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
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
    EXPECT_TRUE(httpRequest.ParseHeaders(emptyHeaders, 0, 0));
}

TEST_F(HttpRequestTest, ParseHeaders_MalformedHeader) {
    std::string malformedHeader1{"thisHeaderIsMalformed"};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.ParseHeaders({malformedHeader1}, 0, 1));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, ParseHeaders_ContentType) {
    std::string contentType{"application/json"};
    std::string header1{"Content-Type: "+contentType};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.ParseHeaders({header1}, 0, 1));

    EXPECT_EQ(contentType, httpRequest.mContentType);
}

TEST_F(HttpRequestTest, ParseHeaders_ContentLength) {
    std::string contentLength{"8"};
    std::string header1{"Content-Length: "+contentLength};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.ParseHeaders({header1}, 0, 1));

    EXPECT_EQ(contentLength, httpRequest.mContentLength);
}

TEST_F(HttpRequestTest, ParseHeaders_BothHeadersPresent) {
    std::string contentLength{"8"};
    std::string header1{"Content-Length: "+contentLength};
    std::string contentType{"application/json"};
    std::string header2{"Content-Type: "+contentType};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.ParseHeaders({header1, header2}, 0, 2));

    EXPECT_EQ(contentLength, httpRequest.mContentLength);
    EXPECT_EQ(contentType, httpRequest.mContentType);
}

TEST_F(HttpRequestTest, ParseHeaders_OtherHeadersIgnored) {
    std::string contentLength{"8"};
    std::string header1{"Content-Length: "+contentLength};
    std::string contentType{"application/json"};
    std::string header2{"Content-Type: "+contentType};
    std::string ignoredValue{"myVal"};
    std::string header3{"Ignore-Me: "+ignoredValue};
    
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.ParseHeaders({header1, header2, header3}, 0, 3));

    EXPECT_EQ(contentLength, httpRequest.mContentLength);
    EXPECT_EQ(contentType, httpRequest.mContentType);
}







TEST_F(HttpRequestTest, Parse_EmtpyRequest) {
    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.Parse(mEmptyRequest));
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
    std::string request{"GET /fake/uri HTTP/1.1\n\n"};
    
    HttpRequest httpRequest(request);
    EXPECT_TRUE(httpRequest.Parse(request));
}

TEST_F(HttpRequestTest, GetRequestEmtpyLineIndex_1) {
    std::vector<std::string> headerVec{"GET /fake/uri HTTP/1.1",
                                       "Content-Type: type",
                                       ""};
    size_t expectedPosition{2}; 

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_EQ(expectedPosition, httpRequest.GetRequestEmptyLineIndex(headerVec));
}

TEST_F(HttpRequestTest, GetRequestEmtpyLineIndex_2) {
    std::vector<std::string> headerVec{"GET /fake/uri HTTP/1.1",
                                       ""};
    size_t expectedPosition{1};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_EQ(expectedPosition, httpRequest.GetRequestEmptyLineIndex(headerVec));
}

TEST_F(HttpRequestTest, GetRequestEmtpyLineIndex_3) {
    std::vector<std::string> headerVec{"GET /fake/uri HTTP/1.1",
                                       "",
                                       "HereIsSomeMockData"};
    size_t expectedPosition{1};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_EQ(expectedPosition, httpRequest.GetRequestEmptyLineIndex(headerVec));
}

TEST_F(HttpRequestTest, GetRequestEmtpyLineIndex_4) {
    std::vector<std::string> headerVec{"",
                                       "HereIsSomeMockData"};
    size_t expectedPosition{0};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_EQ(expectedPosition, httpRequest.GetRequestEmptyLineIndex(headerVec));
}

TEST_F(HttpRequestTest, GetRequestEmtpyLineIndex_5) {
    std::vector<std::string> headerVec{"",
                                       "",
                                       "HereIsSomeMockData"};
    size_t expectedPosition{0};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_EQ(expectedPosition, httpRequest.GetRequestEmptyLineIndex(headerVec));
}

TEST_F(HttpRequestTest, RequestHasDataErrors_GET_Error) {
    std::string verb{"GET"};
    size_t requestSize{3};
    size_t emptyLineIndex{1}; // No data allowed in GET

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.RequestHasDataErrors(verb, requestSize, emptyLineIndex));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, RequestHasDataErrors_GET_Valid_1) {
    std::string verb{"GET"};
    size_t requestSize{2};
    size_t emptyLineIndex{1};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.RequestHasDataErrors(verb, requestSize, emptyLineIndex));
}

TEST_F(HttpRequestTest, RequestHasDataErrors_GET_Valid_2) {
    std::string verb{"GET"};
    size_t requestSize{4};
    size_t emptyLineIndex{3};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.RequestHasDataErrors(verb, requestSize, emptyLineIndex));
}

TEST_F(HttpRequestTest, RequestHasDataErrors_PUT_Error) {
    std::string verb{"POST"};
    size_t requestSize{4};
    size_t emptyLineIndex{3};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_TRUE(httpRequest.RequestHasDataErrors(verb, requestSize, emptyLineIndex));
    EXPECT_EQ(400, httpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, RequestHasDataErrors_PUT_Valid_1) {
    std::string verb{"POST"};
    size_t requestSize{4};
    size_t emptyLineIndex{2};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.RequestHasDataErrors(verb, requestSize, emptyLineIndex));
}

TEST_F(HttpRequestTest, RequestHasDataErrors_PUT_Valid_2) {
    std::string verb{"POST"};
    size_t requestSize{50};
    size_t emptyLineIndex{2};

    HttpRequest httpRequest(mEmptyRequest);
    EXPECT_FALSE(httpRequest.RequestHasDataErrors(verb, requestSize, emptyLineIndex));
}