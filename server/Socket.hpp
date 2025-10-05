#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>

class Socket
{
    /*
        socket() => create a fd linked to the socket
        setsockopt(), bind() => set options of the sock , then linked it to port and domain
    */
    private:
        int sockFd;
        struct sockaddr_in address;
        bool parseIPv4(const std::string& ip, uint32_t& result);


    public:
        Socket(); 
        ~Socket();
        bool create();
        bool bind(std::string);
        bool setNonBlocking();
        bool listen(int backlog);
        int getFd() const ;
        int accept();
};


#endif