#include "request.hpp"
#include "response.hpp"

DirectoryListing::DirectoryListing() : root(nullptr), indexFile(nullptr), hasIndexFile(false), autoIndex(false), locationFound(false) {}

DirectoryListing::~DirectoryListing() {}

void    DirectoryListing::setRoot(const std::string &path)
{
    root = path;
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

void HandleGetResponse(const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{
    DirectoryListing locationConfig;
    if (!req.uri.empty() && req.uri[0] == '/')
    {
        size_t secondSlash = req.uri.find('/', 1);
        if (secondSlash != std::string::npos)
        {
            std::string location = req.uri.substr(0, pos);
            fillReqStruct(nullptr, locationConfig, location, req.headers.at("Host"));
            if (!locationConfig.getRoot())
                buildErroResponse(404);
        }
        else
        {
            fillReqStruct(nullptr, locationConfig, req.uri, req.headers.at("Host"));
            if (!locationConfig.getRoot())
                treatFileRequest(req, responBuffer);
        }
    }
}