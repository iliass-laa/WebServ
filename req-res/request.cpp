#include "request.hpp"

std::vector<char> buildErrorResponse(int status) {
    int code;
    std::string reason;
    switch (status) {
        case ERROR_BAD_METHOD:  code = 501; reason = "Not Implemented"; break;
        case ERROR_BAD_VERSION: code = 505; reason = "HTTP Version Not Supported"; break;
        case ERROR:             code = 400; reason = "Bad Request"; break;
        case 404:               code = 404; reason = "Not Found"; break;
        case 403:               code = 403; reason = "Forbidden"; break;
        case 409:               code = 409; reason = "Conflict"; break;
        case 405:               code = 405; reason = "Method Not Allowed"; break;
        default:                code = 400; reason = "Bad Request"; break;
    }

    std::ostringstream body;
    body << "<html><body><h1>" << code << " " << reason << "</h1></body></html>";
    std::string bodyStr = body.str();

    std::ostringstream ss;
    ss << "HTTP/1.1 " << code << " " << reason << "\r\n";
    ss << "Content-Type: text/html\r\n";
    ss << "Content-Length: " << bodyStr.size() << "\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    ss << bodyStr;

    std::string response = ss.str();
    return std::vector<char>(response.begin(), response.end());
}

// void printRequest(const struct HttpRequest &Req) {
//     std::cout << "Method: " << Req.method << "\n";
//     std::cout << "URI: " << Req.uri << "\n";
//     std::cout << "Version: " << Req.version << "\n";
//     std::cout << "Headers:\n";
//     for (const auto &header : Req.headers) {
//         std::cout << header.first << ": " << header.second << "\n";
//     }
//     std::cout << "Body (" << Req.body.size() << " bytes):\n";
//     std::cout.write(Req.body.data(), Req.body.size());
//     std::cout << "\n";
// }

void printResponse(const std::vector<char> &responseBuffer) {
    std::cout << "Response (" << responseBuffer.size() << " bytes):\n";
    std::cout.write(responseBuffer.data(), responseBuffer.size());
    std::cout << "\n";
}


int handleRequest(BaseNode* ConfigNode, std::vector<char> requestBuffer, std::vector<char> &responseBuffer) {
    struct HttpRequest Req;    
    int status = parseRequest(requestBuffer, Req);
    if (status == INCOMPLETE)
        return INCOMPLETE;
    else if (status == ERROR_BAD_METHOD || status == ERROR_BAD_VERSION || status == ERROR)
        responseBuffer = buildErrorResponse(status);
    // printRequest(Req);
    if (Req.method == "GET")
        HandleGetResponse(ConfigNode, Req, responseBuffer);
    else if (Req.method == "POST")
        HandlePostResponse(ConfigNode, Req, responseBuffer);
    else
        HandleDeleteResponse(ConfigNode, Req, responseBuffer);
    // printResponse(responseBuffer);
    if (Req.headers.at("Connection") == "close")
        return COMPLETE;
    else
        return COMPLETEDEF;
    
}

int parseChunkedBody(std::vector<char> &body) {
    size_t pos = 0;
    std::vector<char> temp;

    while (true) {
        if (pos >= body.size())
            return INCOMPLETE;
        size_t lineEnd = std::string(body.begin() + pos, body.end()).find("\r\n");
        if (lineEnd == std::string::npos)
            return INCOMPLETE;

        std::string sizeStr(body.begin() + pos, body.begin() + pos + lineEnd);
        long chunkSize = strtol(sizeStr.c_str(), NULL, 16);
        pos += lineEnd + 2;
        if (chunkSize == 0) {
            if (pos + 2 > body.size())
                return INCOMPLETE;
            if (body[pos] != '\r' || body[pos + 1] != '\n')
                return ERROR;
            body = temp;
            return COMPLETE;
        }

        if (pos + chunkSize + 2 > body.size())
            return INCOMPLETE;
        temp.insert(temp.end(), body.begin() + pos, body.begin() + pos + chunkSize);

        pos += chunkSize;
        if (body[pos] != '\r' || body[pos + 1] != '\n')
            return ERROR;
        pos += 2;
    }
}

int parseRequest(std::vector<char> requestBuffer, struct HttpRequest &Req) {

    std::string requestString(requestBuffer.begin(), requestBuffer.end());
    size_t pos = requestString.find("\r\n\r\n");
    if (pos == std::string::npos)
        return INCOMPLETE;
    std::string headerPart = requestString.substr(0, pos);
    std::vector<char> bodyPart(requestString.begin() + pos + 4, requestString.end());
    std::istringstream headerStream(headerPart);
    std::string reqLine;
    std::getline(headerStream, reqLine);
    std::istringstream reqLineStream(reqLine);

    reqLineStream >> Req.method >> Req.uri >> Req.version;
    if (Req.method.empty() || Req.uri.empty() || Req.version.empty())
        return ERROR;
    if (Req.method != "GET" && Req.method != "POST" && Req.method != "DELETE")
        return ERROR_BAD_METHOD;
    if (Req.version != "HTTP/1.1")
        return ERROR_BAD_VERSION;
    std::string headerLine;
    while (std::getline(headerStream, headerLine)) {
        if (headerLine == "\r")
            break;
        size_t colonPos = headerLine.find(":");
        if (colonPos == std::string::npos)
            return ERROR;
        std::string key = headerLine.substr(0, colonPos);
        std::string value = headerLine.substr(colonPos + 1);
        while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            value.erase(0,1);
        while (!value.empty() && (value[value.size()-1] == ' ' || value[value.size()-1] == '\t'))
            value.erase(value.size()-1);
        Req.headers[key] = value;
    }
    if (Req.headers.find("Content-Length") != Req.headers.end()) {
        int contentLength = atoi(Req.headers["Content-Length"].c_str());
        if ((int)bodyPart.size() < contentLength)
            return INCOMPLETE;
    }
    else if (Req.headers.find("Transfer-Encoding") != Req.headers.end() &&
             Req.headers["Transfer-Encoding"] == "chunked") {
        int status = (parseChunkedBody(bodyPart));
        if (status != COMPLETE)
            return status;
    }
    Req.body = bodyPart;
    return COMPLETE;
}
