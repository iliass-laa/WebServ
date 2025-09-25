#include "request.hpp"

class DirectoryListing {
    private :
        std::string directoryPath;
        std::vector<char> indexFile;
        bool    hasIndexFile;
        bool   autoIndex;
        bool   isDirectory;
    public :
    DirectoryListing();
    ~DirectoryListing();
    void    setDirectoryPath(const std::string &path);
    void    setAutoIndex(bool value);
    bool    getAutoIndex() const;
    void    setIndexFile(const std::vector<char> &file);
    bool    getHasIndexFile() const;
    std::vector<char>  getIndexFile() const;
};