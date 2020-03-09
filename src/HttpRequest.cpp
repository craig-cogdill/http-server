#include "HttpRequest.h"
#include <sstream>
#include <algorithm>
#include <iostream>

HttpRequest::HttpRequest(std::string rawRequest):
    mValid(true),
    mValidVerbs({"GET", "POST", "DELETE"}),
    mValidHttpVersion("HTTP/1.1"),
    kBadRequestReturnValue(404),
    mContentTypeKey("Content-Type"),
    mContentLengthKey("Content-Length") {
    // yay
}

bool HttpRequest::IsValid() {
    return mValid;
}    
    
std::vector<std::string> HttpRequest::Explode(std::string& s, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim);) {
        result.push_back(std::move(token));
    }

    return result;
}

bool HttpRequest::Parse(std::string rawRequest) {
    std::vector<std::string> requestLines = Explode(rawRequest, '\n');
    if (requestLines.empty()) {
        return false;
    }
    return ParseFirstLine(requestLines.at(0));
}

bool HttpRequest::ParseFirstLine(std::string firstLine) {
    std::vector<std::string> firstLineArgs = Explode(firstLine, ' ');
    if (3 != firstLineArgs.size()) {
        std::cout << "Wrong args: " << firstLineArgs.size() << std::endl;
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
    
bool HttpRequest::ParseHeaders(std::vector<std::string> headers) {
    for (auto& header : headers) {
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