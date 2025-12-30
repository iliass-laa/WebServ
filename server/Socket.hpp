#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
class Socket
{
    /*
        socket() => create a fd linked to the socket
        setsockopt(), bind() => set options of the sock , then linked it to port and domain
    */
    private:
        int sockFd;
        // struct sockaddr_in address;
        struct addrinfo *res;
        struct addrinfo address;
        bool parseIPv4(const std::string& ip, uint32_t& result);


    public:
        Socket(); 
        ~Socket();
        bool create(std::string );
        bool bind();
        bool setNonBlocking();
        bool listen(int );
        int getFd() const ;
        int accept();
};


#endif