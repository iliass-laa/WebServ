# ifndef STRUCTS_HPP
# define STRUCTS_HPP

#include "includes.hpp"
#include "AST.hpp"



enum CgiExtensions
{
    bash,
    php, 
    py,
    NONE
};

enum methods{
    GET,
    POST,
    DELETE,
    NOTALLOWED
};


enum response_Types{
    STATIC,
    REVERSE_PROXY,
    CGI
};

enum errors{
   PATH_NOT_FOUND,
   Port_ERROR,
};

typedef struct s_request_header t_request_header;
typedef struct s_request t_request;
typedef struct s_response t_response;
typedef enum methods t_methods;


struct s_request_header
{
    t_methods method;
    std::string httpV;
    std::string path;
    std::string hostname;
    int port;
    std::map <std::string, std::vector <std::string> > additionalHeaders;
};


struct s_request
{
    ContextNode *server;
    ContextNode *location;
    std::string rootDir;
    t_request_header headers;
    std::string requestBody;
};


struct s_response
{
    std::string responseBody;
};



#endif