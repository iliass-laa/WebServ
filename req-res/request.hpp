#include <iostream>
#include <map>
#include <vector>
#include <sstream>

struct HttpRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
};

enum ParseStatus {
    INCOMPLETE,
    COMPLETE,
    ERROR_BAD_METHOD,
    ERROR_BAD_VERSION,
    ERROR
};