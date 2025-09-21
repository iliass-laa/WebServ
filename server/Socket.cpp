#include"Socket.hpp"

Socket::Socket(): sockFd(-1){}; 

Socket::~Socket(){};

bool Socket::create()
{
    // socket( domian , type, protocol)
    /*
        domian , for webserv use AF_INET
        type , tcp SOCK_STREAM , UDP SOCK_DGRAM
        protocol , not specified is 0 
    */
    sockFd = socket(AF_INET, SOCK_STREAM, 0); // 
    return sockFd != -1;
}

bool Socket::bind(int port)
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // accept from any ip (0.0.0.0)
    address.sin_port = htons(port);

    /*
            set all the info assigned to address struct 
        to the socket pointed by sockFD

        int setsockopt(int socket, int level, int option_name,
            const void *option_value, socklen_t option_len);
        socket , socket to setup
        level   , configure in which layer (see notion)
        option_name ,the option that want to setup (see notion)
        *option_value ,the option value 
        option_len  , option size 

    */
    int opt = 1 ; // 1 enable , 0 disable
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    return ::bind(sockFd, (struct sockaddr*)&address, sizeof(address)) == 0;
}

bool Socket::setNonBlocking()
{
    // for non-blocking i/o 
    /*
        server should be configured with fcntl()
        get the file flags first 
        set it to be non-blocking
    */
    int flags = fcntl(sockFd, F_GETFL, 0);
    return fcntl(sockFd, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool Socket::listen(int backlog)
{
    return ::listen(sockFd, backlog);
}

int Socket::getFd() const 
{
    return sockFd;
}

int Socket::accept()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    return ::accept(sockFd, (struct sockaddr *)&client_addr, &client_len);
}