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
    std::vector<char> read_buffer; // for request
    std::vector<char> write_buffer; // for response
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
