#include "response.hpp"

DirectoryListing::DirectoryListing() : hasIndexFile(false), autoIndex(false), _default(false){}

DirectoryListing::~DirectoryListing() {}

void    DirectoryListing::setRoot(const std::string &path)
{
    root = path;
}

bool    DirectoryListing::getDefault() const
{
    return _default;
}

void    DirectoryListing::setAutoIndex(bool value)
{
    autoIndex = value;
}

bool    DirectoryListing::getAutoIndex() const
{
    return autoIndex;
}

void    DirectoryListing::setIndexFile(const std::vector<std::string> &file)
{
    indexFile = file;
    hasIndexFile = true;
}

bool    DirectoryListing::getHasIndexFile() const
{
    return hasIndexFile;
}

std::vector<std::string>    DirectoryListing::getIndexFile() const
{
    return indexFile;
}


const std::string &DirectoryListing::getRoot() const
{
    return root;
}

std::vector<std::string> DirectoryListing::getAllowedMethods() const
{
    return allowedMethods;
}

std::pair <bool, std::string>   DirectoryListing::getRedirect()
{
    return redirect;
}

void  DirectoryListing::setRedirect(std::pair <bool, std::string> rd)
{
    redirect = rd;
}

void   DirectoryListing::setDefault(bool value)
{
    _default = value;
}

void   DirectoryListing::setAllowedMethods(const std::vector<std::string> &methods)
{
    allowedMethods = methods;
}

int checkPath(const std::string &path)
{
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0)
        return -1;
    return pathStat.st_mode;
}

size_t DirectoryListing::getMaxBodySize() const
{
    return maxBodySize;
}

void   DirectoryListing::setMaxBodySize(size_t size)
{
    maxBodySize = size;
}

void replaceSpaces(std::string &uri)
{
    size_t pos = 0;
    while ((pos = uri.find("%20", pos)) != std::string::npos)
    {
        uri.replace(pos, 3, " ");
        pos += 1;
    }
}

std::string getContentType(const std::string &path)
{
    size_t dotPosition = path.find_last_of('.');
    if (dotPosition == std::string::npos)
        return "application/octet-stream";
    std::string extension = path.substr(dotPosition + 1);
    if (extension == "html" || extension == "htm")
        return "text/html";
    if (extension == "css")
        return "text/css";
    if (extension == "py" || extension == "c" || extension == "cpp")
        return "text/plain";
    if (extension == "csv")
        return "text/csv";
    if (extension == "js")
        return "application/javascript";
    if (extension == "png")
        return "image/png";
    if (extension == "jpg" || extension == "jpeg")
        return "image/jpeg";
    if (extension == "gif")
        return "image/gif";
    if (extension == "mp4")
        return "video/mp4";
    if (extension == "txt")
        return "text/plain";
    if (extension == "mov")
        return "video/quicktime";
    if (extension == "webm")
        return "video/webm";
    return "application/octet-stream";
}

int buildFileResponse(std::string path, std::vector<char> &responseBuffer, const HttpRequest &Req)
{
    (void)Req;
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
        return 1;
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
        return 1;
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Length: " << size << "\r\n";
    responseStream << "Content-Type: " << getContentType(path) << "\r\n";
    // if (Req.cookiesIndex)
    // {
    //     std::map<std::string, std::string>::const_iterator it;
    //     for (it = Req.cookies.begin(); it != Req.cookies.end(); it++)
    //         responseStream << "Set-Cookie: " << it->first << "=" << it->second << "; Path=/" << "\r\n";
    // }
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
    responseBuffer.insert(responseBuffer.end(), buffer.begin(), buffer.end());
    return 0; 
}

void buildRedirectionResponse(const std::string &newLocation, std::vector<char> &responseBuffer, const HttpRequest &Req)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 301 Moved Permanently\r\n";
    responseStream << "Location: " << newLocation << "\r\n";
    responseStream << "Content-Length: 0\r\n";
    if (Req.cookiesIndex)
    {
        std::map<std::string, std::string>::const_iterator it;
        for (it = Req.cookies.begin(); it != Req.cookies.end(); it++)
            responseStream << "Set-Cookie: " << it->first << "=" << it->second << "; Path=/" << "\r\n";
    }
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
}

int buildAutoIndexResponse(const std::string &directoryPath, const std::string &uri, std::vector<char> &responseBuffer, const HttpRequest &Req)
{
    std::vector<std::string> entries;
    DIR *dir = opendir(directoryPath.c_str());
    if (!dir)
        return 1;
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name != "." && name != "..")
        {
            if (S_ISDIR(checkPath(directoryPath + '/' + name)))
                name += '/';
            entries.push_back(name);
        }
    }
    closedir(dir);
    if (entries.empty())
        return 1;
    std::ostringstream BodyStream;
    BodyStream << "<html><head><title>Index of " << uri << "</title></head><body>";
    BodyStream << "<h1>Index of " << uri << "</h1><ul>";
    for (size_t i = 0; i < entries.size(); i++)
    {
        BodyStream << "<li><a href=\"" << uri;
        if (!uri.empty() && uri[uri.size()-1] != '/')
            BodyStream << '/';
        BodyStream << entries[i] << "\">" << entries[i] << "</a></li>";
    }
    BodyStream << "</ul></body></html>";
    std::string body = BodyStream.str();
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Length: " << body.size() << "\r\n";
    responseStream << "Content-Type: text/html\r\n";
    if (Req.cookiesIndex)
    {
        std::map<std::string, std::string>::const_iterator it;
        for (it = Req.cookies.begin(); it != Req.cookies.end(); it++)
            responseStream << "Set-Cookie: " << it->first << "=" << it->second << "; Path=/" << "\r\n";
    }
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
    responseBuffer.insert(responseBuffer.end(), body.begin(), body.end());
    return 0;
}

void HandleGetResponse(BaseNode* ConfigNode, struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    DirectoryListing locationConfig;
    
    // std::cout << Req.uri << std::endl;
    replaceSpaces(Req.uri);
    // std::cout << Req.uri << std::endl;
    std::map <std::string , std::string> ::const_iterator it;
    try{
        it = Req.headers.find("Host");
        if (it != Req.headers.end())
            fillReqStruct(ConfigNode, locationConfig, Req.uri, Req.headers.at("Host"));
        else{
            // std::cerr << "No Host Found While Building a Get Response for this URI :" 
            //         << Req.uri 
            //         <<std::endl;
            ;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "==>'Get Response' Caugth this exception:" << e.what() << "\n";
    }
    std::vector<std::string> allowedMethods = locationConfig.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), "GET") == allowedMethods.end())
    {
        responseBuffer = buildErrorResponse(405);
        return;
    }
    std::string fileSystemPath = locationConfig.getRoot();
    if (fileSystemPath.length() != 0 && fileSystemPath[fileSystemPath.length() - 1] != '/')
        fileSystemPath += '/';
    fileSystemPath += Req.uri.substr(1);
    int pathType = checkPath(fileSystemPath);
    if (pathType == -1)
    {
        responseBuffer = buildErrorResponse(404);
        return;
    }
    if (locationConfig.getRedirect().first)
    {
        buildRedirectionResponse(locationConfig.getRedirect().second, responseBuffer, Req);
        return;
    }
    if (S_ISDIR(pathType))
    {
        if ((!fileSystemPath.empty() && fileSystemPath[fileSystemPath.size() - 1] != '/'))
        {
            buildRedirectionResponse(Req.uri + '/', responseBuffer, Req);
            return;
        }
        if (locationConfig.getHasIndexFile())
        {
            std::vector<std::string> indexFiles = locationConfig.getIndexFile();
            for (size_t i = 0; i < indexFiles.size(); i++)
            {
                std::string indexPath = fileSystemPath + indexFiles[i];
                pathType = checkPath(indexPath);
                std::cout << indexPath;
                if (S_ISREG(pathType))
                {
                    if (!buildFileResponse(indexPath, responseBuffer, Req))
                        return;
                }
            }
        }
        if (locationConfig.getAutoIndex())
        {
            if (!buildAutoIndexResponse(fileSystemPath, Req.uri, responseBuffer, Req))
                return;
        }
        responseBuffer = buildErrorResponse(403);
            return;
    }
    else if (S_ISREG(pathType))
    {
        if (!buildFileResponse(fileSystemPath, responseBuffer, Req))
            return;
    }
    else
    {
        responseBuffer = buildErrorResponse(404);
        return;
    }
}

void buildPostResponse(std::vector<char> &responseBuffer, const HttpRequest &Req)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 201 Created\r\n";
    responseStream << "Content-Length: 32\r\n";
    if (Req.cookiesIndex)
    {
        std::map<std::string, std::string>::const_iterator it;
        for (it = Req.cookies.begin(); it != Req.cookies.end(); it++)
            responseStream << "Set-Cookie: " << it->first << "=" << it->second << "; Path=/" << "\r\n";
    }
    responseStream << "\r\n";
    responseStream << "Resource created successfully.\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
}

void buildProcessResponse(std::vector<char> &responseBuffer, const HttpRequest &Req)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Length: 32\r\n";
    if (Req.cookiesIndex)
    {
        std::map<std::string, std::string>::const_iterator it;
        for (it = Req.cookies.begin(); it != Req.cookies.end(); it++)
            responseStream << "Set-Cookie: " << it->first << "=" << it->second << "; Path=/" << "\r\n";
    }
    responseStream << "\r\n";
    responseStream << "Request processed succesfully.\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
}

void HandlePostResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    bool    writeSuccess = false;
    DirectoryListing locationConfig;
    std::vector<std::string> parts;

    fillReqStruct(ConfigNode, locationConfig, Req.uri, Req.headers.at("Host"));
    std::vector<std::string> allowedMethods = locationConfig.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), "POST") == allowedMethods.end())
    {
        responseBuffer = buildErrorResponse(405);
        return;
    }
    std::string uploadPath;
    if (locationConfig.getRoot() != "/")
        uploadPath = locationConfig.getRoot() + Req.uri;
    else
        uploadPath = Req.uri;
    size_t boundaryPos = Req.headers.at("Content-Type").find("boundary=");
    if (boundaryPos == std::string::npos)
    {
        responseBuffer = buildErrorResponse(415);
        return;
    }
    std::string rawboundary = Req.headers.at("Content-Type").substr(boundaryPos + 9);
    while (!rawboundary.empty() && (rawboundary[rawboundary.size() - 1] == '\r' || rawboundary[rawboundary.size() - 1] == '\n' || rawboundary[rawboundary.size() - 1] == ' '))
        rawboundary.erase(rawboundary.size() - 1);
    while (!rawboundary.empty() && (rawboundary[0] == ' ' || rawboundary[0] == '\r' || rawboundary[0] == '\n'))
       rawboundary.erase(0, 1);
    std::string boundary = "--" + rawboundary;
    std::string endBoundary = boundary + "--";
    std::string body(Req.body.begin(), Req.body.end());
    size_t start = body.find(boundary);
    if (start == std::string::npos)
    {
        responseBuffer = buildErrorResponse(400);
        return;
    }
    start += boundary.size();
    if (body[start] == '\r')
        start++;
    if (body[start] == '\n')
        start++;
    while (true)
    {
        size_t next = body.find(boundary, start);
        size_t nextEnd = body.find(endBoundary, start);
        if (nextEnd != std::string::npos && (next == std::string::npos || nextEnd < next)) 
        {
            parts.push_back(body.substr(start, nextEnd - start));
            break;
        }
        if (next == std::string::npos)
            break;
        parts.push_back(body.substr(start, next - start));
        start = next + boundary.size();
        if (body[start] == '\r')
            start++;
        if (body[start] == '\n')
            start++;
    }
    if (parts.empty())
    {
        responseBuffer = buildErrorResponse(400);
        return;
    }
    for (size_t i = 0; i < parts.size(); i++)
    {
        size_t headerEnd = parts[i].find("\r\n\r\n");
        if (headerEnd == std::string::npos)
            continue;
        std::string headersPart = parts[i].substr(0, headerEnd);
        std::string bodyPart = parts[i].substr(headerEnd + 4);
        if (bodyPart.size() >= 2 && bodyPart.substr(bodyPart.size() - 2) == "\r\n")
            bodyPart.resize(bodyPart.size() - 2);
        std::istringstream headerStream(headersPart);
        std::string headerLine;
        std::string fileName;
        while (std::getline(headerStream, headerLine))
        {
            if (headerLine.find("Content-Disposition:") != std::string::npos)
            {
                size_t namePos = headerLine.find("filename=\"");
                if (namePos != std::string::npos)
                {
                    size_t nameEnd = headerLine.find("\"", namePos + 10);
                    if (nameEnd != std::string::npos)
                        fileName = headerLine.substr(namePos + 10, nameEnd - (namePos + 10));
                }
            }
        }
        if (fileName.empty())
            continue;
        std::string fullPath = uploadPath + '/' + fileName;
        std::ofstream outFile(fullPath.c_str(), std::ios::binary);
        if (!outFile)
        {
            std::cout << "Failed to open file for writing: " << fullPath << std::endl;
            responseBuffer = buildErrorResponse(500);
            return;
        }
        outFile.write(bodyPart.c_str(), bodyPart.size());
        writeSuccess = true;
        outFile.close();
    }
    if (writeSuccess)
        buildPostResponse(responseBuffer, Req);
    else
        buildProcessResponse(responseBuffer, Req);
}

void buildDelResponse(std::vector<char> &responseBuffer,const HttpRequest &Req)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n";
    responseStream << "Content-Length: 19\r\n";
    if (Req.cookiesIndex)
    {
        std::map<std::string, std::string>::const_iterator it;
        for (it = Req.cookies.begin(); it != Req.cookies.end(); it++)
            responseStream << "Set-Cookie: " << it->first << "=" << it->second << "; Path=/" << "\r\n";
    }
    responseStream << "\r\n";
    responseStream << "Resource deleted.\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
}

void HandleDeleteResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    DirectoryListing locationConfig;

    fillReqStruct(ConfigNode, locationConfig, Req.uri, Req.headers.at("Host"));
    std::vector<std::string> allowedMethods = locationConfig.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), "DELETE") == allowedMethods.end())
    {
        responseBuffer = buildErrorResponse(405);
        return;
    }
    std::string fileSystemPath = locationConfig.getRoot();
    if (fileSystemPath.length() != 0 && fileSystemPath[fileSystemPath.length() - 1] != '/')
        fileSystemPath += '/';
    fileSystemPath += Req.uri.substr(1);
    int pathType = checkPath(fileSystemPath);
    if (pathType == -1)
    {
        responseBuffer = buildErrorResponse(404);
        return;
    }
    if (S_ISREG(pathType))
    {
        if (remove(fileSystemPath.c_str()) != 0)
        {
            responseBuffer = buildErrorResponse(500);
            return;
        }
        buildDelResponse(responseBuffer, Req);
            return;
    }
    else if (S_ISDIR(pathType))
    {
       if (rmdir(fileSystemPath.c_str()) != 0)
       {
           responseBuffer = buildErrorResponse(409);
           return;
       }
       buildDelResponse(responseBuffer, Req);
       return;
    }
    else
    {
        responseBuffer = buildErrorResponse(404);
        return;
    }
}