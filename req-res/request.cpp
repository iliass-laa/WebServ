#include "request.hpp"

std::vector<char> buildErrorResponse(int status) {
    int code;
    std::string reason;
    switch (status) {
        case ERROR_BAD_METHOD:      code = 501; reason = "Not Implemented"; break;
        case ERROR_BAD_VERSION:     code = 505; reason = "HTTP Version Not Supported"; break;
        case ERROR:                 code = 400; reason = "Bad Request"; break;
        case ERROR_BODY_TOO_LARGE:  code = 413; reason = "Payload Too Large"; break;
        case 404:                   code = 404; reason = "Not Found"; break;
        case 415:                   code = 415; reason = "Unsupported Media Type"; break;
        case 403:                   code = 403; reason = "Forbidden"; break;
        case 409:                   code = 409; reason = "Conflict"; break;
        case 405:                   code = 405; reason = "Method Not Allowed"; break;
        case 500:                   code = 500; reason = "Internal Server Error"; break;
        default:                    code = 400; reason = "Bad Request"; break;
    }

    std::ostringstream body;
    body << "<html><body><h1>" << code << " " << reason << "</h1></body></html>";
    std::string bodyStr = body.str();

    std::ostringstream ss;
    ss << "HTTP/1.1 " << code << " " << reason << "\r\n";
    ss << "Content-Type: text/html\r\n";
    ss << "Content-Length: " << bodyStr.size() << "\r\n";
    ss << "\r\n";
    ss << bodyStr;

    std::string response = ss.str();
    return std::vector<char>(response.begin(), response.end());
}

void printRequest(const struct HttpRequest &Req) {
    std::cout << "Method: " << Req.method << "\n";
    std::cout << "URI: " << Req.uri << "\n";
    std::cout << "Version: " << Req.version << "\n";
    std::cout << "Headers:\n";
    for (const auto &header : Req.headers) {
        std::cout << header.first << ": " << header.second << "\n";
    }
    std::cout << "Body (" << Req.body.size() << " bytes):\n";
    std::cout.write(Req.body.data(), Req.body.size());
    std::cout << "\n";
}

void printResponse(const std::vector<char> &responseBuffer) {
    std::cout << "Response (" << responseBuffer.size() << " bytes):\n";
    std::cout.write(responseBuffer.data(), responseBuffer.size());
    std::cout << "\n";
}


int handleRequest(BaseNode* ConfigNode, std::vector<char> &requestBuffer, std::vector<char> &responseBuffer, struct HttpRequest &Req) {
    // std::clock_t startTime, endTime;
    // startTime = std::clock();
    int status = parseRequest(ConfigNode, requestBuffer, Req);
    // endTime = std::clock();
    // double seconds = double(endTime - startTime) / CLOCKS_PER_SEC;
    // std::cout << std::fixed << std::setprecision(6);
    // std::cout << "Time taken outside parse: " << seconds << " seconds" << std::endl;
    if (status == INCOMPLETE)
        return INCOMPLETE;
    else if (status != COMPLETE && status != COMPLETEDEF)
        responseBuffer = buildErrorResponse(status);
    printRequest(Req);
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

bool isChunkedBodyComplete(const std::vector<char>& buf)
{
    const char* endChunk = "0\r\n\r\n";
    const size_t endChunkLen = 5;

    if (buf.size() < endChunkLen)
        return false;
    size_t scanStart = (buf.size() > 50) ? buf.size() - 50 : 0;

    for (size_t i = scanStart; i <= buf.size() - endChunkLen; ++i)
    {
        if (std::memcmp(&buf[i], endChunk, endChunkLen) == 0)
            return true;
    }
    return false;
}

int parseRequest(BaseNode *ConfigNode, std::vector<char> &requestBuffer, struct HttpRequest &Req) {

    // std::clock_t startTime, checkTime, endTime;
    // startTime = std::clock();  
    (void)ConfigNode;
    if (!Req.headerParsed)
    {
        // std::cout << "RequestBuffer = " << std::string(requestBuffer.begin(), requestBuffer.end()) << std::endl;
        std::string requestString(requestBuffer.begin(), requestBuffer.end());
        Req.headerEndPos = requestString.find("\r\n\r\n");
        if (Req.headerEndPos == std::string::npos)
            return INCOMPLETE;
        std::string headerPart = requestString.substr(0, Req.headerEndPos);
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
            while (!value.empty() && (value[value.size()-1] == ' ' || value[value.size()-1] == '\t' || value[value.size()-1] == '\r' || value[value.size()-1] == '\n'))
                value.erase(value.size()-1);
            Req.headers[key] = value;
            // Req.maxBodySize = getMaxBodySize(ConfigNode, Req.uri);
        }
        if (Req.headers.find("Transfer-Encoding") != Req.headers.end() &&
            Req.headers["Transfer-Encoding"] == "chunked")
            Req.isChunked = true;
        Req.headerParsed = true;
    }
    // if (Req.isChunked)
    // {
    //     if (!isChunkedBodyComplete(std::vector<char>(requestBuffer.begin() + Req.headerEndPos + 4, requestBuffer.end())))
    //         return INCOMPLETE;
    // }
    if (Req.headers.find("Content-Length") != Req.headers.end()) {
        Req.contentLength = std::strtoul(Req.headers.at("Content-Length").c_str(), NULL, 10);  
            // if ((size_t)(requestBuffer.end() - (requestBuffer.begin() + Req.headerEndPos + 4)) > Req.maxBodySize)
            //     return ERROR_BODY_TOO_LARGE;
            if ((size_t)(requestBuffer.end() - (requestBuffer.begin() + Req.headerEndPos + 4)) < Req.contentLength)
            {
                // checkTime = std::clock();
                // double seconds = double(checkTime - startTime) / CLOCKS_PER_SEC;
                // std::cout << std::fixed << std::setprecision(6);
                // std::cout << "Time taken to parse request: " << seconds << " seconds" << std::endl;
                return INCOMPLETE;
            }
                
    }
    std::vector<char> bodyPart(requestBuffer.begin() + Req.headerEndPos + 4, requestBuffer.end());
    if (Req.isChunked){
        int status = (parseChunkedBody(bodyPart));
        if (status != COMPLETE)
            return status;
    }
    Req.body = bodyPart;
    // endTime = std::clock();
    // double seconds = double(endTime - startTime) / CLOCKS_PER_SEC;
    // std::cout << std::fixed << std::setprecision(6);
    // std::cout << "Time taken to parse request: " << seconds << " seconds" << std::endl;
    return COMPLETE;
}

// int parseRequest(std::vector<char> requestBuffer, struct HttpRequest &Req, bool &headerParsed, size_t &headerEnd) {

//     const char *sep = "\r\n\r\n";
//     const char *sep_end = sep + 4;

//     std::vector<char>::const_iterator it = std::search(
//         requestBuffer.begin(), requestBuffer.end(),
//         sep, sep_end
//     );

//     if (it == requestBuffer.end())
//         return INCOMPLETE;

//     size_t headerEnd = it - requestBuffer.begin();

//     std::string headerPart(requestBuffer.begin(), requestBuffer.begin() + headerEnd);
//     size_t pos = headerPart.find("Content-Length");
//     size_t contentLength = 0;
//     if (pos != std::string::npos) {
//         size_t colonPos = headerPart.find(':', pos);
//         size_t newLinePos = headerPart.find('\n', colonPos);
//         std::string contentLengthString = headerPart.substr(colonPos + 1, newLinePos - (colonPos + 1));
//         contentLengthString.erase(0, contentLengthString.find_first_not_of(" \t\r"));
//         contentLengthString.erase(contentLengthString.find_last_not_of(" \t\r") + 1);
//         contentLength = std::strtoul(contentLengthString.c_str(), NULL, 10);
//     }
//     size_t bodyStart = headerEnd + 4;
//     if (contentLength && requestBuffer.size() - bodyStart < contentLength)
//         return INCOMPLETE;

//     std::vector<char> bodyPart(requestBuffer.begin() + bodyStart, requestBuffer.end());
//     std::istringstream headerStream(headerPart);
//     std::string reqLine;
//     std::getline(headerStream, reqLine);
//     std::istringstream reqLineStream(reqLine);
//     reqLineStream >> Req.method >> Req.uri >> Req.version;

//     if (Req.method.empty() || Req.uri.empty() || Req.version.empty())
//         return ERROR;
//     if (Req.method != "GET" && Req.method != "POST" && Req.method != "DELETE")
//         return ERROR_BAD_METHOD;
//     if (Req.version != "HTTP/1.1")
//         return ERROR_BAD_VERSION;

//     std::string headerLine;
//     while (std::getline(headerStream, headerLine)) {
//         if (headerLine == "\r")
//             break;
//         size_t colonPos = headerLine.find(':');
//         if (colonPos == std::string::npos)
//             return ERROR;
//         std::string key = headerLine.substr(0, colonPos);
//         std::string value = headerLine.substr(colonPos + 1);
//         while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
//             value.erase(0, 1);
//         while (!value.empty() && (value[value.size()-1] == ' ' || value[value.size()-1] == '\t'))
//             value.erase(value.size()-1);
//         Req.headers[key] = value;
//     }

//     if (Req.headers.find("Transfer-Encoding") != Req.headers.end() &&
//         Req.headers["Transfer-Encoding"] == "chunked") {
//         int status = parseChunkedBody(bodyPart);
//         if (status != COMPLETE)
//             return status;
//     }
//     Req.body = bodyPart;
//     return COMPLETE;
// }

