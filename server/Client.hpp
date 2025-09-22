#ifndef CLIENT_HPP
#define CLIENT_HPP
#include<string>
#include<vector>
#include <sys/socket.h>
#define BUFFER 4096

class Client {
private:
    int client_fd;
    bool connected;
    std::vector<char> reqBuff; // for request
    std::string resBuff; // for response
    ssize_t resOffset;
public:
    Client(int);
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
};
#endif // CLIENT_HPP
