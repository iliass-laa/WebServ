#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <sys/stat.h>

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

int		parseRequest(std::vector<char> requestBuffer, struct HttpRequest &Req);
void	HandleGetResponse(const struct HttpRequest &Req, std::vector<char> &responseBuffer);
std::vector<char> buildErrorResponse(int status);