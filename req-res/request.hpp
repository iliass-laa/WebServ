#include <iostream>
#include <map>
#include <vector>
#include <sstream>

struct HttpRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};
