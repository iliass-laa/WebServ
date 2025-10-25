#ifndef CLIENT_HPP
#define CLIENT_HPP
#include<string>
#include<iostream>
#include<vector>
#include <sys/socket.h>
#include "../req-res/request.hpp"
#define BUFFER 4096

class BaseNode;


struct HttpRequest {
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
    std::string boundary;
    size_t contentLength;
    size_t headerEndPos;
    bool   headerParsed;
    bool   isChunked;
};

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
    
public:
    Client(int, BaseNode*);
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

};
#endif // CLIENT_HPP
