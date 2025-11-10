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
    std::string resBuff; // for response
    std::vector<char> respoBuff; // for response
    ssize_t resOffset;
    ssize_t responseSize;
    int state;
    bool keepAlive;
    struct HttpRequest Req;
    bool isCGI;
    
public:
    Client(int, BaseNode* ,Core&);
    ~Client();
    Client(const Client& other);
    Client& operator=(const Client& other);
    bool isConnected() const ;
    int getFd() const ;
    bool writeData();
    bool readData();
    void queueResponse(const std::string &response);
    bool hasDataToRead() const ;
    bool hasDataToWrite() const ;
    std::string getReadBuffer();  
    std::vector<char>& getRespoBuffer();  
    std::string getResBuffer();
    void setReqStruct (struct HttpRequest& req);
    struct HttpRequest& getReqStruct ();
    void setResponseSize(ssize_t );

    void setClientState(int );
    int getClientState() const;

    void setKeepAlive(int );
    int getKeepAlive() const;
    void clearReqStruct();
    void clearVectReq();
    void clearVectRes();
    cgiHandling &getCGI(){return cgi;};
    bool getIsCGI(){return isCGI;};
};
#endif // CLIENT_HPP
