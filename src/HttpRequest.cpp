#include "HttpRequest.h"
#include <sstream>
#include <algorithm>
#include <cstring>

HttpRequest::HttpRequest():
    mVerb(""),
    mUri(""),
    mContentType(""),
    mContentLength(""),
    mRequestData(""),
    mCRLF("\r\n"),
    mValid(false),
    mValidVerbs({"GET", "POST", "DELETE"}),
    mValidHttpVersion("HTTP/1.1"),
    kBadRequestReturnValue(404),
    mContentTypeKey("Content-Type"),
    mContentLengthKey("Content-Length"),
    mHeaderDelimiter(": ") {
}

HttpRequest::HttpRequest(const char* rawRequest): HttpRequest() {
    if (nullptr != rawRequest && rawRequest[0] != '\0') {
        std::vector<std::string> requestLines = GetLinesOfRawRequestAndCacheData(rawRequest);
        if (!requestLines.empty()) {
            std::string firstLine(requestLines.at(0));
            requestLines.erase(requestLines.begin());
            mValid = CacheFirstLineRequestArgs(firstLine)
                && !RequestHasDataErrors(GetVerb(), GetData())
                && CacheHeaders(requestLines);
        }
    }
}

bool HttpRequest::IsValid() {
    return mValid;
}    

int HttpRequest::GetBadRequestReturnCode() {
    return kBadRequestReturnValue;
}    

std::string HttpRequest::GetVerb() {
    return mVerb;
}

std::string HttpRequest::GetUri() {
    return mUri;
}
    
std::string HttpRequest::GetData() {
    return mRequestData;
}

std::string HttpRequest::GetContentLength() {
    return mContentLength;
}

std::string HttpRequest::GetContentType() {
    return mContentType;
}
    
std::vector<std::string> HttpRequest::Explode(std::string& s, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim);) {
        result.push_back(std::move(token));
    }

    return result;
}

// This function breaks the Single Responsibility Principle, as it not only
//   returns the first line and headers of the request, it is also caches the
//   data field of the request (if it exists).
//
// This is being done purposefully, as parsing the raw request character by
//   character is something that should only be done once. It is a special
//   case of parsing this field that should you get two consecutive CRLF, everything
//   left to parse is the data field in its entirety.
//
// The SRP is violated to take advantage of this special case while the data is
//   already being parsed.
std::vector<std::string> HttpRequest::GetLinesOfRawRequestAndCacheData(const char* rawRequest) {
    int charsSinceLastCR(0);
    int startNewWordAt(0);
    std::vector<std::string> requestLines;
    for (size_t i = 0; i < strlen(rawRequest); i++) {
        if (rawRequest[i] == '\r') {
            // Detect a double CRLF and save the rest of the string to the data field
            if (charsSinceLastCR == 1) {
                int dataStartIdx = i + 2;
                mRequestData = std::string(&rawRequest[dataStartIdx], strlen(rawRequest)-dataStartIdx);
                break;
            } else {
                charsSinceLastCR = 0;
                if (i+1 != strlen(rawRequest)) {
                    if (rawRequest[i+1] == '\n') {
                        std::string line(&rawRequest[startNewWordAt], i-startNewWordAt);
                        requestLines.push_back(line);
                        startNewWordAt = i+2;
                        continue;
                    }
                }
            }
        }
        charsSinceLastCR++; 
    }
    return requestLines;
}

bool HttpRequest::CacheFirstLineRequestArgs(std::string firstLine) {
    std::vector<std::string> firstLineArgs = Explode(firstLine, ' ');
    if (3 != firstLineArgs.size()) {
        kBadRequestReturnValue = 400;
        return false;        
    }
    
    // Verify HTTP/1.1
    if (firstLineArgs.at(2) != mValidHttpVersion) {
        kBadRequestReturnValue = 400;
        return false;
    }
    
    // Verify verb
    std::string verb(firstLineArgs.at(0));
    if (std::find(mValidVerbs.begin(), mValidVerbs.end(), verb) == mValidVerbs.end()) {
        kBadRequestReturnValue = 405;
        return false;
    }

    // Store valid request
    mVerb = verb;
    mUri = firstLineArgs.at(1);
    return true;
}
    
bool HttpRequest::CacheHeaders(std::vector<std::string> headers) {
    if (!headers.empty()) {
        for (auto& header : headers) {
            size_t delimIdx = header.find(mHeaderDelimiter);
            if (std::string::npos == delimIdx) {
                kBadRequestReturnValue = 400;
                return false;
            }
            // Store desired headers, ignore the rest
            std::string headerKey(header.substr(0, delimIdx));
            std::string headerVal(header.substr(delimIdx+2, header.length()));
            if (headerKey == mContentTypeKey) {
                mContentType = headerVal;
            } else if (headerKey == mContentLengthKey) {
                mContentLength = headerVal;
            }
        }
    }
    return true;
}
    
bool HttpRequest::RequestHasDataErrors(const std::string& verb, const std::string& requestData) {
    if ("GET" == verb && !requestData.empty()) {
        // A GET request can have headers, but no data
        kBadRequestReturnValue = 400;
        return true;
    } else if ("DELETE" == verb && !requestData.empty()) {
        // A DELETE request can have no additional headers or data
        kBadRequestReturnValue = 400;
        return true;
    } else if ("POST" == verb && requestData.empty()) {
        // A POST request must have data
        kBadRequestReturnValue = 400;
        return true;
    }
    return false;
}