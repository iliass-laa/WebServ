#include "response.hpp"

DirectoryListing::DirectoryListing() : root(nullptr), indexFile(nullptr), hasIndexFile(false), autoIndex(false), locationFound(false) {}

DirectoryListing::~DirectoryListing() {}

void    DirectoryListing::setRoot(const std::string &path)
{
    root = path;
}

bool    DirectoryListing::getDefault() const
{
    return locationFound;
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

bool   DirectoryListing::getUploadSupport() const
{
    return uploadSupport;
}

void   DirectoryListing::setUploadSupport(bool value)
{
    uploadSupport = value;
}

void DirectoryListing::setDefault(bool value)
{
    locationFound = value;
}              

int checkPath(const std::string &path)
{
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0)
        return -1;
    return pathStat.st_mode;
}


std::string getContentType(const std::string &path)
{
    size_t dotPosition = path.find_last_of('.');
    if (dotPos == std::string::npos)
        return "application/octet-stream";
    std::string extension = path.substr(dotPos + 1);
    if (extension == "html" || extension == "htm")
        return "text/html";
    if (extension == "css")
        return "text/css";
    if (extension == "js")
        return "application/javascript";
    if (extension == "png")
        return "image/png";
    if (extension == "jpg" || extension == "jpeg")
        return "image/jpeg";
    if (extension == "gif")
        return "image/gif";
    if (extension == "txt")
        return "text/plain";
    return "application/octet-stream";
}

int buildFileResponse(std::string path, std::vector<char> &responseBuffer)
{
    std::ifstream file(path, std::ios::binary);
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
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
    responseBuffer.insert(responseBuffer.end(), buffer.begin(), buffer.end());
    return 0;
}

void buildRedirectionResponse(const std::string &newLocation, std::vector<char> &responseBuffer)
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 301 Moved Permanently\r\n";
    responseStream << "Location: " << newLocation << "\r\n";
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
}

int buildAutoIndexResponse(const std::string &directoryPath, const std::string &uri, std::vector<char> &responseBuffer)
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
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
    responseBuffer.insert(responseBuffer.end(), body.begin(), body.end());
    return 0;
}

void HandleGetResponse(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    DirectoryListing locationConfig;
    std::string longestMatchingPrefix = Req.uri;
    while (true)
    {
        fillReqStruct(ConfigNode, locationConfig, longestMatchingPrefix, Req.headers.at("Host"));
        if (!locationConfig.getDefault())
            break;
        size_t pos = Req.uri.find_last_of('/');
        if (pos == std::string::npos || pos == 0)
            break;
        longestMatchingPrefix = Req.uri.substr(0, pos);
    }
    std::string fileSystemPath = locationConfig.getRoot() + Req.uri;
    if (S_ISDIR(checkPath(fileSystemPath)))
    {
        if ((!fileSystemPath.empty() && fileSystemPath[fileSystemPath.size() - 1] != '/'))
        {
            buildRedirectionResponse(Req.uri + '/', responseBuffer);
            return;
        }
        if (locationConfig.getHasIndexFile())
        {
            std::vector<std::string> indexFiles = locationConfig.getIndexFile();
            for (size_t i = 0; i < indexFiles.size(); i++)
            {
                std::string indexPath = fileSystemPath + '/' + indexFiles[i];
                if (S_ISREG(checkPath(indexPath)))
                {
                    if (!buildFileResponse(indexPath, responseBuffer))
                        return;
                }
            }
        }
        if (locationConfig.getAutoIndex())
        {
            if (!buildAutoIndexResponse(fileSystemPath, Req.uri, responseBuffer))
                return;
        }
        responseBuffer = buildErrorResponse(403);
            return;
    }
    else if (S_ISREG(checkPath(fileSystemPath)))
    {
        if (!buildFileResponse(fileSystemPath, responseBuffer))
            return;
    }
    else
    {
        responseBuffer = buildErrorResponse(404);
        return;
    }
}

void handlePostRequest(BaseNode* ConfigNode, const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    DirectoryListing locationConfig;
    std::vector<std::string> parts;

    fillReqStruct(ConfigNode, locationConfig, Req.uri, Req.headers.at("Host"));
    if (!locationConfig.getUploadSupport())
    {
        responseBuffer = buildErrorResponse(403);
        return;
    }
    if (locationConfig.getRoot() != "/")
        std::string uploadPath = locationConfig.getRoot() + Req.uri;
    size_t boundaryPos = Req.headers.at("Content-Type").find("Boundary=");
    if (boundaryPos != std::string::npos)
    {
        responseBuffer = buildErrorResponse(400);
        return;
    }
    Req.boundary = Req.headers.at("Content-Type").substr(pos + 9);
    while (true)
    {
        size_t next = Req.body.find(Req.boundary, boundaryPos + 9);
        if (next == std::string::npos)
            break;
        parts.push_back(std::string(Req.body.begin() + boundaryPos + 9, Req.body.begin() + next));
        boundaryPos = next;
    }
    

}