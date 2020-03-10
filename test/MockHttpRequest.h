#pragma once
#include "HttpRequest.h"

class MockHttpRequest: public HttpRequest {
public:
    MockHttpRequest(): HttpRequest() {}
    virtual ~MockHttpRequest() = default;

    // This function declaration is identical to the HttpRequest class.
    //   It is not virtual by design, since the HttpRequest function is
    //   called in the HttpRequest constructor.
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
};