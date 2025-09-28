#ifndef CLASSES_HPP
#define CLASSES_HPP 


#include "includes.hpp"

class ConfigFileError : public std::exception {
private:
    std::string msg; 
public:
    const char* what() const throw(){  
        return msg.c_str();
    }
    
    ConfigFileError() : msg("Need a valid Config File Name") {}
    
    ConfigFileError(const std::string& message) : msg(message) {}
    virtual ~ConfigFileError()throw(){};
};

class RequestError : public std::exception {
private:
    std::string msg; 
public:
    const char* what() const throw(){  
        return msg.c_str();
    }
    
    RequestError() : msg("Invalid Request! (Only HTTP1.1 or HTTP1.0)") {}
    
    RequestError(const std::string& message) : msg(message) {}
    virtual ~RequestError()throw(){};
};


class LocationConfig
{
    public :
        LocationConfig():path("NULL"), root("NULL"), index("NULL"){};
        ~LocationConfig(){};
    
        std::string path;
        std::string root;
        std::string index;
};




class ServerConfig
{
    public :
        ServerConfig():port(-1), serverName("NULL"){};
        ~ServerConfig(){};


        int port;
        std::string serverName;
};


class ConfigFileInfos : public LocationConfig , public ServerConfig
{
    public :
        ConfigFileInfos(){};
        ~ConfigFileInfos(){};
};  



#endif