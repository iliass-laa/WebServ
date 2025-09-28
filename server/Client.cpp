#include "Client.hpp"

Client::Client(int fd):client_fd(fd), connected(true), resOffset(0) {
    // Constructor implementation
}

Client::~Client() {
    // Destructor implementation
}

Client::Client(const Client& other) {
    // Copy constructor implementation
    (void)other;
}

Client& Client::operator=(const Client& other) {
    // Copy assignment operator implementation
    if (this != &other) {
        // Perform deep copy
    }
    return *this;
}

int Client::getFd() const {
    return client_fd;
}

bool Client::isConnected() const {
    return connected;
}

bool Client::hasDataToRead() const 
{ 
    return !reqBuff.size();
}

bool Client::hasDataToWrite() const 
{ 
    return !resBuff.size(); 
}

bool Client::readData(){
    char buffer[BUFFER];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0); // 
    if (bytes > 0){
        std::cout << buffer << std::endl;


        
        std::vector<char> readed(buffer, buffer + bytes);
        reqBuff.insert(reqBuff.end(), readed.begin(), readed.end());
        return true;
    }
    else if (bytes == 0){
        connected = false; // Client disconnected
    }
    return false;
}

bool Client::writeData(){ // to vector<char> of write_data
    
    if(!resBuff.size() || !connected)
        return false;
    resOffset = send(client_fd, resBuff.c_str(), BUFFER,0);
    if (resOffset > 0)
        return true;
    return false;
}

void Client::queueResponse(const std::string &response)// based on what the responce is , will be sended as string
{
    // add the responce to client resBuff
    resBuff += response ;
}

std::string Client::getReadBuffer(){
    return std::string(reqBuff.begin(), reqBuff.end());
}

