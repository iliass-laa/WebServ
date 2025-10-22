#ifndef CLIENT_HPP
#define CLIENT_HPP
#include<string>
#include<iostream>
#include<vector>
#include <sys/socket.h>
#include "../req-res/request.hpp"
#define BUFFER 5000000

class BaseNode;

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
    void setResponseSize(ssize_t );

    void setClientState(int );
    int getClientState() const;

    void setKeepAlive(int );
    int getKeepAlive() const;
    void clearVectReq();
    void clearVectRes();

};
#endif // CLIENT_HPP
