#pragma once

#include "../parsing/headers/includes.hpp"

class Client;
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
    bool   cookiesIndex;
    std::map<std::string, std::string> cookies;
    size_t maxBodySize;
    Client *thisClient;
};
