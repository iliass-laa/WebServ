#include "request.hpp"
#include "response.hpp"

DirectoryListing::DirectoryListing() : hasIndexFile(false), autoIndex(false), isDirectory(false) {}

DirectoryListing::~DirectoryListing() {}

void    DirectoryListing::setDirectoryPath(const std::string &path)
{
    directoryPath = path;
}

void    DirectoryListing::setAutoIndex(bool value)
{
    autoIndex = value;
}

bool    DirectoryListing::getAutoIndex() const
{
    return autoIndex;
}

void    DirectoryListing::setIndexFile(const std::vector<char> &file)
{
    indexFile = file;
    hasIndexFile = true;
}

bool    DirectoryListing::getHasIndexFile() const
{
    return hasIndexFile;
}

void HandleGetResponse(const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{

}