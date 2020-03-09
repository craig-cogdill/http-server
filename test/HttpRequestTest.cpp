#include "HttpRequestTest.h"
#include "HttpRequest.h"

TEST_F(HttpRequestTest, Explode_emptyString) {
    std::string emptyString("");
    char delim{'\n'};

    HttpRequest httpRequest(mEmptyRequest.c_str());
    std::vector<std::string> explodedStr = httpRequest.Explode(emptyString, delim);
    EXPECT_TRUE(explodedStr.empty());
}

TEST_F(HttpRequestTest, Explode_MultiLineString) {
    std::string firstElement{"hello"};
    std::string secondElement{"world"};
    std::string testString{firstElement+"\n"+secondElement};
    char delim{'\n'};

    HttpRequest httpRequest(mEmptyRequest.c_str());
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

    HttpRequest httpRequest(mEmptyRequest.c_str());
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

    HttpRequest httpRequest(mEmptyRequest.c_str());
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

    HttpRequest httpRequest(mEmptyRequest.c_str());
    std::vector<std::string> explodedStr = httpRequest.Explode(testString, delim);
    
    EXPECT_FALSE(explodedStr.empty());
    ASSERT_TRUE(2 == explodedStr.size());
    EXPECT_EQ(firstElement, explodedStr.at(0));
    EXPECT_EQ(secondElement, explodedStr.at(1));
}

TEST_F(HttpRequestTest, Parse_FirstLineOnly) {
    std::string expectedVerb("GET");
    std::string expectedUri("/test/uri");
    std::string httpVersion("HTTP/1.1");
    std::string onlyFirstLine(expectedVerb+" "+expectedUri+" "+httpVersion+"\r\n");

    HttpRequest httpRequest(onlyFirstLine.c_str());
    EXPECT_EQ(expectedVerb, httpRequest.GetVerb());
}