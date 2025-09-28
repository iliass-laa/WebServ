#include "request.hpp"
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
    responseStream << "Content-Type: " getContentType(path) << "\r\n";
    responseStream << "\r\n";
    std::string responseHeaders = responseStream.str();
    responseBuffer.insert(responseBuffer.end(), responseHeaders.begin(), responseHeaders.end());
    return 0;
}

void HandleGetResponse(const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    DirectoryListing locationConfig;
    std::string longestMatchingPrefix = Req.uri;
    while (true)
    {
        fillReqStruct(nullptre, locationConfig, longestMatchingPrefix, Req.headers.at("Host"));
        if (locationConfig.getDefault())
            break;
        size_t pos = Req.uri.find_last_of('/');
        if (pos == std::string::npos || pos == 0)
            break;
        longestMatchingPrefix = Req.uri.substr(0, pos);
    }
    std::string fileSystemPath = locationConfig.getRoot() + Req.uri;
    if (S_ISDIR(checkPath(fileSystemPath)))
    {
        if (fileSystemPath.end() != '/' )
            buildRedirectionResponse(fileSystemPath + '/');
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
        else
        {
            responseBuffer = buildErrorResponse(403);
            return;
        }
    }
    else if (S_ISREG(checkPath(fileSystemPath)))
    {
        if (!buildFileResponse(indexPath, responseBuffer))
            return;
    }
    else
    {
        responseBuffer = buildErrorResponse(404);
        return;
    }
}