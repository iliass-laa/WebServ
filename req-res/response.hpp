#include "request.hpp"
#include "../parsing/headers/webserver.hpp"

class DirectoryListing {
    private :
        std::string root;
        std::vector<std::string> indexFile;
        bool    hasIndexFile;
        bool   autoIndex;
    public :
    DirectoryListing();
    ~DirectoryListing();
    void    setDirectoryPath(const std::string &path);
    void    setAutoIndex(bool value);
    bool    getAutoIndex() const;
    void    setIndexFile(const std::vector<std::string> &file);
    bool    getHasIndexFile() const;
    bool    getLocationFound() const;
};