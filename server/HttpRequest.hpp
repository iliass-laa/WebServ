#pragma once

#include "../parsing/headers/includes.hpp"

struct HttpRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
    std::string boundary;
    size_t contentLength;
    size_t headerEndPos;
    bool   headerParsed;
    bool   isChunked;
    size_t maxBodySize;
};
