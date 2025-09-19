#include "Client.hpp"

Client::Client(int fd):client_fd(fd), connected(true) {
    // Constructor implementation
}

Client::~Client() {
    // Destructor implementation
}

Client::Client(const Client& other) {
    // Copy constructor implementation
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

bool Client::writeData(){
    char buffer[BUFFER];

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes > 0)
    {
        /*
            std::vector<int> AB = A;
            AB.insert(AB.end(), B.begin(), B.end());
        */
       std::vector<char> readed(buffer, buffer + bytes);
       read_buffer.insert(read_buffer.end(), readed.begin(), readed.end());
       
        return true;
    }
    else if (bytes == 0)
    {
        connected = false; // Client disconnected
    }
    // else if (errno != EAGAIN && errno != EWOULDBLOCK)
    // {
    //     connected = false; // Error occurred
    // }
    return false;
}

bool Client::readData(){

}