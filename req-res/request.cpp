#include "request.hpp"
#include "../server/Client.hpp"
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


int handleRequest(BaseNode* ConfigNode, std::vector<char> &requestBuffer, std::vector<char> &responseBuffer, struct HttpRequest &Req) {
    int status;
    status = parseRequest(ConfigNode, requestBuffer, Req);
    if (status == INCOMPLETE)
        return INCOMPLETE;
    else if (status != COMPLETE && status != COMPLETEDEF)
        responseBuffer = buildErrorResponse(status);
    if (!Req.uri.compare(0, 9, "/cgi-bin/"))
            return CGI;
    
    if (Req.method == "GET")
        HandleGetResponse(ConfigNode, Req, responseBuffer);
    else if (Req.method == "POST")
        HandlePostResponse(ConfigNode, Req, responseBuffer);
     else
        HandleDeleteResponse(ConfigNode, Req, responseBuffer);


    std::map <std::string, std::string> ::iterator it;
    it= Req.headers.find("Connection");
    if (it == Req.headers.end())
        std::cout << "";
    if (it == Req.headers.end() || Req.headers.at("Connection") == "close")
    { 
        std::cout<< GREEN <<"AAALLO\n" << DEF;
        return COMPLETE;
    }
    else
    {
        std::cout<< GREEN <<"WAAALLO\n" << DEF;
        return COMPLETEDEF; 
    }
}

int parseChunkedBody(std::vector<char> &body) {
    size_t pos = 0;
    std::vector<char> result;
    result.reserve(body.size());

    while (pos < body.size()) {
        size_t lineEnd = pos;
        while (lineEnd + 1 < body.size() && !(body[lineEnd] == '\r' && body[lineEnd + 1] == '\n'))
            ++lineEnd;
        if (lineEnd + 1 >= body.size())
            return ERROR;
        std::string sizeStr(body.begin() + pos, body.begin() + lineEnd);
        long chunkSize = strtol(sizeStr.c_str(), NULL, 16);
        pos = lineEnd + 2;

        if (chunkSize == 0) {
            if (pos + 1 >= body.size() || body[pos] != '\r' || body[pos + 1] != '\n')
                return ERROR;
            body.swap(result);
            return COMPLETE;
        }
        if (pos + chunkSize + 1 >= body.size())
            return ERROR;
        result.insert(result.end(), body.begin() + pos, body.begin() + pos + chunkSize);
        pos += chunkSize + 2;
    }
    return ERROR;
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
        if (memcmp(&buf[i], endChunk, endChunkLen) == 0)
            return true;
    }
    return false;
}

void parseCookies(std::string Cookies, std::map<std::string, std::string> &cookiesMap)
{
    std::istringstream cookieStream(Cookies);
    std::string cookiePair;
    while (std::getline(cookieStream, cookiePair, ';')) {
        size_t equalPos = cookiePair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = cookiePair.substr(0, equalPos);
            std::string value = cookiePair.substr(equalPos + 1);
            while (!key.empty() && (key[0] == ' ' || key[0] == '\t'))
                key.erase(0, 1);
            while (!key.empty() && (key[key.size() - 1] == ' ' || key[key.size() - 1] == '\t'))
                key.erase(key.size() - 1);
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
                value.erase(0, 1);
            while (!value.empty() && (value[value.size() - 1] == ' ' || value[value.size() - 1] == '\t'))
                value.erase(value.size() - 1);
            cookiesMap[key] = value;
        }
    }
}

int parseRequest(BaseNode *ConfigNode, std::vector<char> &requestBuffer, struct HttpRequest &Req) {

    std::map <std::string, std::string> ::iterator it;
    it= Req.headers.find("Connection");
    if (it == Req.headers.end())
        std::cout << "Connection Not found \n";
    if (!Req.headerParsed)
    {
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
     
        if (Req.uri.compare("/.well-known/appspecific/com.chrome.devtools.json") == 0)
        {
            return ERROR;
        }

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

            getMaxBodySize(ConfigNode, Req.maxBodySize, Req.headers.at("Host"));
        }
        if (Req.headers.find("Transfer-Encoding") != Req.headers.end() &&
            Req.headers["Transfer-Encoding"] == "chunked")
            Req.isChunked = true;
        if (Req.headers.find("Cookie") != Req.headers.end())
        {
            parseCookies(Req.headers["Cookie"], Req.cookies);
            Req.cookiesIndex = true;
            //implement funct fettah
            Req.cookiesIndex = Req.thisClient->handelSession();
            //fix your shit akhay fettah o mat9issch liya lcode diali frfrf
        }
        Req.headerParsed = true;
    }
    if ((size_t)(requestBuffer.end() - (requestBuffer.begin() + Req.headerEndPos + 4)) > Req.maxBodySize)
        return ERROR_BODY_TOO_LARGE;
    if (Req.isChunked)
    {
        if (!isChunkedBodyComplete(requestBuffer))
            return INCOMPLETE;
    }
    if (Req.headers.find("Content-Length") != Req.headers.end()) {
        Req.contentLength = std::strtoul(Req.headers.at("Content-Length").c_str(), NULL, 10);  
            if ((size_t)(requestBuffer.end() - (requestBuffer.begin() + Req.headerEndPos + 4)) < Req.contentLength)
            {
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
    return COMPLETE;
}
