#include "HttpRequest.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstring>



HttpRequest::HttpRequest(const char* rawRequest):
    mVerb(""),
    mUri(""),
    mContentType(""),
    mContentLength(""),
    mRequestData(""),
    mCRLF("\r\n"),
    mValid(true),
    mValidVerbs({"GET", "POST", "DELETE"}),
    mValidHttpVersion("HTTP/1.1"),
    kBadRequestReturnValue(404),
    mContentTypeKey("Content-Type"),
    mContentLengthKey("Content-Length") {
    std::vector<std::string> requestLines = GetLinesOfRawRequestAndCacheData(rawRequest);
}

HttpRequest::HttpRequest():
    mVerb(""),
    mUri(""),
    mContentType(""),
    mContentLength(""),
    mRequestData(""),
    mCRLF("\r\n"),
    mValid(true),
    mValidVerbs({"GET", "POST", "DELETE"}),
    mValidHttpVersion("HTTP/1.1"),
    kBadRequestReturnValue(404),
    mContentTypeKey("Content-Type"),
    mContentLengthKey("Content-Length") {
}

bool HttpRequest::IsValid() {
    return mValid;
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
    
int HttpRequest::GetBadRequestReturnCode() {
    return kBadRequestReturnValue;
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
    //int wordnumber(0);
    for (size_t i = 0; i < strlen(rawRequest); i++) {
        if (rawRequest[i] == '\r') {
            // Detect a double CRLF and save the rest of the string to the data field
            if (charsSinceLastCR == 1) {
                //std::cout << "HERE STARTS THE DOUBLE CRLF" << std::endl;
                //std::cout << "DATA FIELD:" << std::endl;
                int dataStartIdx = i + 2;
                mRequestData = std::string(&rawRequest[dataStartIdx], strlen(rawRequest)-dataStartIdx);
                //std::cout << mRequestData << std::endl;
                break;
            } else {
                charsSinceLastCR = 0;
                if (i+1 != strlen(rawRequest)) {
                    if (rawRequest[i+1] == '\n') {
                        //std::cout << "Found one: " << i << std::endl;
                        std::string line(&rawRequest[startNewWordAt], i-startNewWordAt);
                        requestLines.push_back(line);
                        startNewWordAt = i+2;
                        //std::cout << "**" << line << "**" << std::endl;
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
        std::cout << "Invalid first line args: " << firstLineArgs.size() << std::endl;
        kBadRequestReturnValue = 400;
        return false;        
    }
    
    // Verify HTTP/1.1
    if (firstLineArgs.at(2) != mValidHttpVersion) {
        kBadRequestReturnValue = 400;
        return false;
    }
    
    // Verify verb
    std::string verb = firstLineArgs.at(0);
    std::cout << verb << std::endl;
    if (std::find(mValidVerbs.begin(), mValidVerbs.end(), verb) == mValidVerbs.end()) {
        std::cout << "Invalid verb: " << verb << std::endl;
        kBadRequestReturnValue = 405;
        return false;
    }

    // Store valid request
    mVerb = verb;
    mUri = firstLineArgs.at(1);
    return true;
}
    
bool HttpRequest::ParseHeaders(std::vector<std::string> headers, int start, int end) {
    //auto first = headers.begin() + start;
    //auto last = headers.begin() + end;
    for (int i = 0; i < end; i++) {
        auto header = headers.at(i);
        std::vector<std::string> explodedHeader = Explode(header, ':');
        if (2 != explodedHeader.size()) {
            kBadRequestReturnValue = 400;
            return false;
        }
        // Store desired headers
        std::string headerKey = explodedHeader.at(0);
        std::string headerVal = explodedHeader.at(1).erase(0,1); // Remove leading space
        if (headerKey == mContentTypeKey) {
            mContentType = headerVal;
        } else if (headerKey == mContentLengthKey) {
            mContentLength = headerVal;
        }
    }
    return true;
}
    
size_t HttpRequest::GetRequestEmptyLineIndex(std::vector<std::string> requestLines) {
    return std::distance(requestLines.begin(), std::find(requestLines.begin(), requestLines.end(), ""));
}
    
bool HttpRequest::RequestHasDataErrors(const std::string& verb, const size_t& requestSize, const size_t& emptyStringIdx) {
    if ("GET" == verb && emptyStringIdx != requestSize-1) {
    // A GET request can have headers, but no data
        kBadRequestReturnValue = 400;
        return true;
    } else if ("DELETE" == verb && emptyStringIdx > 1) {
    // A DELETE request can have no additional headers or data
        kBadRequestReturnValue = 400;
        return true;
    } else if ("POST" == verb && emptyStringIdx == requestSize-1) {
    // A POST request must have data
        kBadRequestReturnValue = 400;
        return true;
    }
    return false;
}