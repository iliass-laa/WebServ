#include "request.hpp"
#include "response.hpp"

DirectoryListing::DirectoryListing() {}

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

std::vector<char>  DirectoryListing::getIndexFile() const
{
    return indexFile;
}

void HandleGetResponse(const struct HttpRequest &Req, std::vector<char> &responseBuffer)
{

}