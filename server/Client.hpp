#ifndef CLIENT_HPP
#define CLIENT_HPP
#include<string>
#include<iostream>
#include<vector>
#include <sys/socket.h>
#include "../req-res/request.hpp"
#include "../parsing/headers/webserver.hpp"
#include "Core.hpp"
#include "HttpRequest.hpp"
#include "../parsing/CGI/cgi.hpp"
#define BUFFER 4096

class BaseNode;
class Core;
class cgiHandling;



typedef enum clientState {
    READING_REQUEST, // 0
    WAITTING_FOR_REQUEST, // 2
    SENDING_RESPONSE, // 1
    WAITTING_FOR_RESPONSE, // 2
    INACTIVE,
    WAITTING,
} cState;
class Client {
private:
    BaseNode* root;
    Core& theBase;
    cgiHandling cgi;
    int client_fd;
    bool connected;
    std::vector<char> reqBuff; // for request
    std::string resBuffString; // for response
    std::vector<char> respoBuff; // for response
    ssize_t resOffset;
    int state;
    struct HttpRequest Req;
    bool isCGI;
    bool requestReaded;
    
public:
    Client(int, BaseNode* ,Core&);
    ~Client();
    Client(const Client& other);
    Client& operator=(const Client& other);
    bool isConnected() const ;
    int getFd() const ;
    bool writeData();
    bool readData();
    std::vector<char>& getRespoBuffer();  
    std::string getresBuffStringer();
    void setReqStruct (struct HttpRequest& req);
    struct HttpRequest& getReqStruct ();

    void setClientState(int );
    int getClientState() const;

    void clearReqStruct();
    void clearVectReq();
    void clearVectRes();
    cgiHandling &getCGI(){return cgi;};
    bool getIsCGI(){return isCGI;};
    void setRequestReaded(bool);
    bool getRequestReaded(){return requestReaded;};
};
#endif // CLIENT_HPP
