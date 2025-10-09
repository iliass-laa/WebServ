#ifndef CLIENT_HPP
#define CLIENT_HPP
#include<string>
#include<iostream>
#include<vector>
#include <sys/socket.h>
#include "../req-res/request.hpp"
#define BUFFER 4096

class BaseNode;
class Client {
private:
    BaseNode* root;
    int client_fd;
    bool connected;
    std::vector<char> reqBuff; // for request
    std::string resBuff; // for response
    std::vector<char> respoBuff; // for response
    ssize_t resOffset;
    
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
};
#endif // CLIENT_HPP
