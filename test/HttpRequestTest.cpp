#include "HttpRequestTest.h"
#include "MockHttpRequest.h"

TEST_F(HttpRequestTest, Explode_emptyString) {
    std::string emptyString("");
    char delim{'\n'};

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> explodedStr = mockHttpRequest.Explode(emptyString, delim);
    EXPECT_TRUE(explodedStr.empty());
}

TEST_F(HttpRequestTest, Explode_MultiLineString) {
    std::string firstElement{"hello"};
    std::string secondElement{"world"};
    std::string testString{firstElement+"\n"+secondElement};
    char delim{'\n'};

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> explodedStr = mockHttpRequest.Explode(testString, delim);
    
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

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> explodedStr = mockHttpRequest.Explode(testString, delim);
    
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

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> explodedStr = mockHttpRequest.Explode(testString, delim);
    
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

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> explodedStr = mockHttpRequest.Explode(testString, delim);
    
    EXPECT_FALSE(explodedStr.empty());
    ASSERT_TRUE(2 == explodedStr.size());
    EXPECT_EQ(firstElement, explodedStr.at(0));
    EXPECT_EQ(secondElement, explodedStr.at(1));
}

TEST_F(HttpRequestTest, GetRawRequestLinesAndCacheData_SingleLine) {
    std::string onlyFirstLine("This is the first line received");
    std::string rawRequest(onlyFirstLine+"\r\n");
    size_t expectedLines(1);

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> requestLines = mockHttpRequest.GetLinesOfRawRequestAndCacheData(rawRequest.c_str());
    EXPECT_EQ(expectedLines, requestLines.size());
    EXPECT_EQ(onlyFirstLine, requestLines.at(0));
}

TEST_F(HttpRequestTest, GetRawRequestLinesAndCacheData_MultiLines) {
    std::string firstLine("This is the first line received");
    std::string secondLine("This is the second line received");
    std::string rawRequest(firstLine+"\r\n"+secondLine+"\r\n");
    size_t expectedLines(2);

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> requestLines = mockHttpRequest.GetLinesOfRawRequestAndCacheData(rawRequest.c_str());
    EXPECT_EQ(expectedLines, requestLines.size());
    EXPECT_EQ(firstLine, requestLines.at(0));
    EXPECT_EQ(secondLine, requestLines.at(1));
}

TEST_F(HttpRequestTest, GetRawRequestLinesAndCacheData_MultiLinesWithDoubleCRLF) {
    std::string firstLine("This is the first line received");
    std::string secondLine("This is the second line received");
    std::string rawRequest(firstLine+"\r\n"+secondLine+"\r\n\r\n");
    size_t expectedLines(2);

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> requestLines = mockHttpRequest.GetLinesOfRawRequestAndCacheData(rawRequest.c_str());
    EXPECT_EQ(expectedLines, requestLines.size());
    EXPECT_EQ(firstLine, requestLines.at(0));
    EXPECT_EQ(secondLine, requestLines.at(1));
}

TEST_F(HttpRequestTest, GetRawRequestLinesAndCacheData_MultiLinesWithDataLine) {
    std::string firstLine("This is the first line received");
    std::string secondLine("This is the second line received");
    std::string dataLine("This is example data from the request");
    std::string rawRequest(firstLine+"\r\n"+secondLine+"\r\n\r\n"+dataLine);
    size_t expectedLines(2);

    MockHttpRequest mockHttpRequest;
    std::vector<std::string> requestLines = mockHttpRequest.GetLinesOfRawRequestAndCacheData(rawRequest.c_str());
    EXPECT_EQ(expectedLines, requestLines.size());
    EXPECT_EQ(firstLine, requestLines.at(0));
    EXPECT_EQ(secondLine, requestLines.at(1));
    EXPECT_EQ(dataLine, mockHttpRequest.GetData());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_empty) {
    std::string emptyString("");
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.CacheFirstLineRequestArgs(emptyString));
    EXPECT_EQ(emptyString, mockHttpRequest.GetData());
    EXPECT_EQ(emptyString, mockHttpRequest.GetVerb());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_VerbOnly) {
    std::string verbOnly("GET");
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.CacheFirstLineRequestArgs(verbOnly));
    EXPECT_TRUE(mockHttpRequest.GetVerb().empty());
    EXPECT_EQ(400, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_VerbAndUriOnly) {
    std::string verbAndUriOnly("GET /test/uri");
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.CacheFirstLineRequestArgs(verbAndUriOnly));
    EXPECT_TRUE(mockHttpRequest.GetVerb().empty());
    EXPECT_EQ(400, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_InvalidHttpVersion) {
    std::string firstLine("GET /test/uri HTTP/wrong/version");
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.CacheFirstLineRequestArgs(firstLine));
    EXPECT_TRUE(mockHttpRequest.GetVerb().empty());
    EXPECT_EQ(400, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_BadVerb_PUT) {
    std::string firstLine("PUT /test/uri HTTP/1.1");
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.CacheFirstLineRequestArgs(firstLine));
    EXPECT_TRUE(mockHttpRequest.GetVerb().empty());
    EXPECT_EQ(405, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_BadVerb_HEAD) {
    std::string firstLine("HEAD /test/uri HTTP/1.1");
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.CacheFirstLineRequestArgs(firstLine));
    EXPECT_TRUE(mockHttpRequest.GetVerb().empty());
    EXPECT_EQ(405, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_Valid_GET) {
    std::string expectedVerb("GET");
    std::string expectedUri("/test/uri");
    std::string validHttpVersion("HTTP/1.1");
    std::string verbAndUriOnly(expectedVerb+" "+expectedUri+" "+validHttpVersion);
    MockHttpRequest mockHttpRequest;
    EXPECT_TRUE(mockHttpRequest.CacheFirstLineRequestArgs(verbAndUriOnly));
    EXPECT_EQ(expectedVerb, mockHttpRequest.GetVerb());
    EXPECT_EQ(expectedUri, mockHttpRequest.GetUri());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_Valid_POST) {
    std::string expectedVerb("POST");
    std::string expectedUri("/test/uri");
    std::string validHttpVersion("HTTP/1.1");
    std::string verbAndUriOnly(expectedVerb+" "+expectedUri+" "+validHttpVersion);
    MockHttpRequest mockHttpRequest;
    EXPECT_TRUE(mockHttpRequest.CacheFirstLineRequestArgs(verbAndUriOnly));
    EXPECT_EQ(expectedVerb, mockHttpRequest.GetVerb());
    EXPECT_EQ(expectedUri, mockHttpRequest.GetUri());
}

TEST_F(HttpRequestTest, CacheFirstLineRequestArgs_Valid_DELETE) {
    std::string expectedVerb("DELETE");
    std::string expectedUri("/test/uri");
    std::string validHttpVersion("HTTP/1.1");
    std::string verbAndUriOnly(expectedVerb+" "+expectedUri+" "+validHttpVersion);
    MockHttpRequest mockHttpRequest;
    EXPECT_TRUE(mockHttpRequest.CacheFirstLineRequestArgs(verbAndUriOnly));
    EXPECT_EQ(expectedVerb, mockHttpRequest.GetVerb());
    EXPECT_EQ(expectedUri, mockHttpRequest.GetUri());
}

TEST_F(HttpRequestTest, RequestHasDataErrors_GET_WithData) {
    std::string verb("GET");
    std::string data("Test Data");
    
    MockHttpRequest mockHttpRequest;
    EXPECT_TRUE(mockHttpRequest.RequestHasDataErrors(verb, data));
    EXPECT_EQ(400, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, RequestHasDataErrors_POST_WithNoData) {
    std::string verb("POST");
    std::string emptyData("");
    
    MockHttpRequest mockHttpRequest;
    EXPECT_TRUE(mockHttpRequest.RequestHasDataErrors(verb, emptyData));
    EXPECT_EQ(400, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, RequestHasDataErrors_DELETE_WithData) {
    std::string verb("DELETE");
    std::string data("Test Data");
    
    MockHttpRequest mockHttpRequest;
    EXPECT_TRUE(mockHttpRequest.RequestHasDataErrors(verb, data));
    EXPECT_EQ(400, mockHttpRequest.GetBadRequestReturnCode());
}

TEST_F(HttpRequestTest, RequestHasDataErrors_GET_NoData) {
    std::string verb("GET");
    std::string emptyData("");
    
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.RequestHasDataErrors(verb, emptyData));
}

TEST_F(HttpRequestTest, RequestHasDataErrors_DELETE_NoData) {
    std::string verb("DELETE");
    std::string emptyData("");
    
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.RequestHasDataErrors(verb, emptyData));
}

TEST_F(HttpRequestTest, RequestHasDataErrors_POST_WithData) {
    std::string verb("POST");
    std::string emptyData("Test Data");
    
    MockHttpRequest mockHttpRequest;
    EXPECT_FALSE(mockHttpRequest.RequestHasDataErrors(verb, emptyData));
}