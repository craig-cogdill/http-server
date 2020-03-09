#pragma once
#include <string>
#include <vector>

class HttpRequest {
public:
    HttpRequest(std::string rawRequest);
    virtual ~HttpRequest() = default;

    bool IsValid();
    bool Parse(std::string rawRequest);
    bool ParseFirstLine(std::string firstLine);
    bool ParseHeaders(std::vector<std::string> headers, int start, int end);
    int GetBadRequestReturnCode();
    size_t GetRequestEmptyLineIndex(std::vector<std::string> requestLines);
    bool RequestHasDataErrors(const std::string& verb, const size_t& requestSize, const size_t& emptyStringIdx);
    std::vector<std::string> Explode(std::string& s, char delim);

    std::string mVerb;
    std::string mUri;
    std::string mContentType;
    std::string mContentLength;

protected:
    bool mValid;
    std::vector<std::string> mValidVerbs;
    std::string mValidHttpVersion;
    int kBadRequestReturnValue;
    std::string mContentTypeKey;
    std::string mContentLengthKey;
};