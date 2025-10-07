#pragma once
#include "request.hpp"

class DirectoryListing {
    private :
        std::string root;
        std::vector<std::string> indexFile;
        bool    hasIndexFile;
        bool   autoIndex;
        bool   _default;
        bool    uploadSupport;
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
    bool   getUploadSupport() const;
    void   setUploadSupport(bool value);
};


void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host);