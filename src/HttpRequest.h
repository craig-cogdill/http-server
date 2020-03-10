#pragma once
#include <string>
#include <vector>

class HttpRequest {
public:
    HttpRequest(const char* rawRequest);
    virtual ~HttpRequest() = default;

    bool IsValid();
    int GetBadRequestReturnCode();
    
    std::string GetVerb();
    std::string GetData();
    std::string GetUri();




    bool ParseHeaders(std::vector<std::string> headers, int start, int end);
    size_t GetRequestEmptyLineIndex(std::vector<std::string> requestLines);
    std::vector<std::string> Explode(std::string& s, char delim);


protected:
    // This constructor exists so that unit tests can avoid the constructor parsing
    HttpRequest();

    std::vector<std::string> GetLinesOfRawRequestAndCacheData(const char* rawRequest);
    bool RequestHasDataErrors(const std::string& verb, const std::string& requestData);
    
    std::string mVerb;
    std::string mUri;
    std::string mContentType;
    std::string mContentLength;
    std::string mRequestData;
    
    bool CacheFirstLineRequestArgs(std::string firstLine);
    std::string mCRLF;
    bool mValid;
    std::vector<std::string> mValidVerbs;
    std::string mValidHttpVersion;
    int kBadRequestReturnValue;
    std::string mContentTypeKey;
    std::string mContentLengthKey;
};