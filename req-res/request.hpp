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

enum ParseStatus {
    INCOMPLETE,
    COMPLETE,
    COMPLETEDEF,
    ERROR_BAD_METHOD,
    ERROR_BAD_VERSION,
    ERROR
};

int		parseRequest(std::vector<char> requestBuffer, struct HttpRequest &Req);
void	HandleGetResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer);
std::vector<char> buildErrorResponse(int status);
int handleRequest(BaseNode* ConfigNode, std::vector<char> requestBuffer, std::vector<char> &responseBuffer, struct HttpRequest &Req); ;
void    HandlePostResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer);
void HandleDeleteResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer);
