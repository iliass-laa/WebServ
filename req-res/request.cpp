#include "request.hpp"


struct HttpRequest parseRequest(std::vector<char> requestBuffer){
    struct HttpRequest Req;

    std::string requestString(requestBuffer.begin(), requestBuffer.end());
    size_t pos = requestString.find("\r\n\r\n");
    std::string headerPart = requestString.substr(0, pos);
    std::string bodyPart = requestString.substr(pos + 4);
    std::istringstream headerStream(headerPart);
    std::string reqLine;
    std::getline(headerStream, reqLine);
    std::istringstream reqLineStream(reqLine);

    reqLineStream >> Req.method >> Req.uri >> Req.version;
    std::string headerLine;
    while (std::getline(headerStream, headerLine) && headerLine != "\r") {
        //parse header
    }
}