#include "request.hpp"

int handleRequest(std::vector<char> requestBuffer){
    struct HttpRequest Req;
    
    int status = parseRequest(requestBuffer, Req);
    if (status == INCOMPLETE)
        return INCOMPLETE;
    else if (status == ERROR)
        return ERROR;
    return 0;
}

int parseRequest(std::vector<char> requestBuffer, struct HttpRequest &Req) {

    std::string requestString(requestBuffer.begin(), requestBuffer.end());
    size_t pos = requestString.find("\r\n\r\n");
    if (pos == std::string::npos)
        return INCOMPLETE;
    std::string headerPart = requestString.substr(0, pos);
    std::string bodyPart = requestString.substr(pos + 4);
    std::istringstream headerStream(headerPart);
    std::string reqLine;
    std::getline(headerStream, reqLine);
    std::istringstream reqLineStream(reqLine);

    reqLineStream >> Req.method >> Req.uri >> Req.version;
    if (Req.method.empty() || Req.uri.empty() || Req.version.empty())
        return ERROR;
    if (Req.method != "GET" && Req.method != "POST" && Req.method != "DELETE")
        return ERROR;
    std::string headerLine;
    while (std::getline(headerStream, headerLine)) {
        if (headerLine == "\r")
            break;
        size_t colonPos = headerLine.find(":");
        if (colonPos == std::string::npos)
            return ERROR;
        std::string key = headerLine.substr(0, colonPos);
        std::string value = headerLine.substr(colonPos + 1);
        if (!key.empty() && key[0] == ' ')
            key.erase(0, 1);
        if (!value.empty() && value[value.size()- 1] == '\r')
            value.erase(value.size() - 1);
        Req.headers[key] = value;
    }
    if (Req.headers.find("Content-Length") != Req.headers.end()) {
        int contentLength = atoi(Req.headers["Content-Length"].c_str());
        if ((int)bodyPart.size() < contentLength)
            return INCOMPLETE;
    }
    else if (Req.headers.find("Transfer-Encoding") != Req.headers.end() &&
             Req.headers["Transfer-Encoding"] == "chunked") {
        if (!isChunkedBodyComplete(bodyPart)) //Chunked body parse
            return INCOMPLETE;
    }
    Req.body = bodyPart;
    return COMPLETE;
}