#pragma once
#include "HttpRequest.h"

class MockHttpRequest: public HttpRequest {
public:
    MockHttpRequest(): HttpRequest() {
        SetValid(true);
    }

    virtual ~MockHttpRequest() = default;

    std::vector<std::string> Explode(std::string& str, char delim) {
        return HttpRequest::Explode(str, delim);
    }

    std::vector<std::string> GetLinesOfRawRequestAndCacheData(const char* rawRequest) {
        return HttpRequest::GetLinesOfRawRequestAndCacheData(rawRequest);
    }

    bool CacheFirstLineRequestArgs(std::string firstLine) {
        return HttpRequest::CacheFirstLineRequestArgs(firstLine);
    }
    
    bool RequestHasDataErrors(const std::string& verb, const std::string& requestData) {
        return HttpRequest::RequestHasDataErrors(verb, requestData);
    }
    
    bool CacheHeaders(std::vector<std::string> headers) {
        return HttpRequest::CacheHeaders(headers);
    }

    // Setters to override functionality in tests

    void SetValid(bool isValid) {
        mValid = isValid;
    }
    
    void SetUri(const std::string& uri) {
        mUri = uri;
    }
    
    void SetVerb(const std::string& verb) {
        mVerb = verb;
    }
    
    void SetContentType(const std::string& contentType) {
        mContentType = contentType;
    }

    void SetData(const std::string& data) {
        mRequestData = data;
    }
    
    void SetContentLength(const std::string& contentLength) {
        mContentLength = contentLength;
    }

    void SetBadRequestReturnCode(int retVal) {
        kBadRequestReturnValue = retVal;
    }
};