#pragma once
#include "request.hpp"


class DirectoryListing {
    private :
        std::string root;
        std::vector<std::string> indexFile;
        std::pair <bool, std::string> redirect;
        bool    hasIndexFile;
        bool   autoIndex;
        bool   _default; //is it correspond to when it's location / { ... } // yes ??
        std::vector<std::string> allowedMethods;
        size_t maxBodySize;
    public :
    DirectoryListing();
    ~DirectoryListing();
    void    setRoot(const std::string &path);
    const std::string &getRoot() const;
    void    setAutoIndex(bool value);
    bool    getAutoIndex() const;
    void    setIndexFile(const std::vector<std::string> &file);
    bool    getHasIndexFile() const;
    std::vector<std::string>    getIndexFile() const;
    bool   getDefault() const;
    void   setDefault(bool value);
    std::vector<std::string> getAllowedMethods() const;
    void   setAllowedMethods(const std::vector<std::string> &methods);
    size_t getMaxBodySize() const;
    void   setMaxBodySize(size_t size);
    std::pair <bool, std::string>   getRedirect();
    void   setRedirect(std::pair <bool, std::string>);
};


void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host);