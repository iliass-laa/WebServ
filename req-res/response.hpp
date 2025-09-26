#include "request.hpp"

class DirectoryListing {
    private :
        std::string directoryPath;
        std::vector<std::string> indexFile;
        bool    hasIndexFile;
        bool   autoIndex;
        bool   isDirectory;
    public :
    DirectoryListing();
    ~DirectoryListing();
    void    setDirectoryPath(const std::string &path);
    void    setAutoIndex(bool value);
    bool    getAutoIndex() const;
    void    setIndexFile(const std::vector<std::string> &file);
    bool    getHasIndexFile() const;
    
};