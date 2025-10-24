#pragma once
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include "../parsing/headers/webserver.hpp"
#include "../parsing/headers/AST.hpp"

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
};

enum ParseStatus {
    INCOMPLETE,
    COMPLETE,
    COMPLETEDEF,
    ERROR_BAD_METHOD,
    ERROR_BAD_VERSION,
    ERROR
};

int		parseRequest(std::vector<char> requestBuffer, struct HttpRequest &Req, bool &headerParsed);
void	HandleGetResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer);
std::vector<char> buildErrorResponse(int status);
int handleRequest(BaseNode* ConfigNode, std::vector<char> requestBuffer, std::vector<char> &responseBuffer) ;
void    HandlePostResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer);
void HandleDeleteResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer);
