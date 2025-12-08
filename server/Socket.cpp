#include "Socket.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>


Socket::Socket() : sockFd(-1) {};

Socket::~Socket() {};

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

bool Socket::bind(std::string pair)
{
    size_t colonPos = pair.find(':');
    std::string interface = pair.substr(0, colonPos);
    std::string port = pair.substr(colonPos + 1);
    uint32_t inter;
    parseIPv4(interface, inter);
    address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t)atoi(port.c_str()));
    address.sin_addr.s_addr = inter;
    if (!interface.compare("0.0.0.0"))
        address.sin_addr.s_addr = INADDR_ANY; // accept from any ip (0.0.0.0)

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
    int opt = 1; // 1 enable , 0 disable
    setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    return ::bind(sockFd, (struct sockaddr *)&address, sizeof(address)) == 0;
}

bool Socket::setNonBlocking()
{
    // for non-blocking i/o
    /*
        server should be configured with fcntl()
        get the file flags first
        set it to be non-blocking
    */
    return fcntl(sockFd, F_SETFL, O_NONBLOCK) == 0;
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

bool Socket::parseIPv4(const std::string &ip, uint32_t &result)
{
    std::vector<std::string> numbers;
    std::string curr;

    for (size_t i = 0; i < ip.length(); i++)
    {
        if (ip[i] == '.')
        {
            if (curr.empty())
                return (std::cout << "error 0" << std::endl, false);
            numbers.push_back(curr);
            curr.clear();
        }
        else
            curr.push_back(ip[i]);
    }
    if (!curr.empty())
        numbers.push_back(curr);
    if (numbers.size() != 4)
        return (std::cout << "error 1" << std::endl, false);

    uint32_t parts[4];
    for (int i = 0; i < 4; i++)
    {
        int val = 0;
        for (size_t j = 0; j < numbers[i].length(); j++)
        {
            char c = numbers[i][j];
            if (c < '0' || c > '9')
                return false;
            val = val * 10 + (c - 48);
        }
        parts[i] = val;
    }
    result = ((parts[3] << 24) | (parts[2] << 16) | (parts[1] << 8) | parts[0]);
    return true;
}

// void test(uint32_t result){

//     uint32_t parts[4];
//     for (size_t i = 0; i < 4; ++i) {
//         // Convert string to int manually (C++98 compatible)
//         int val = 0;
//         for (size_t j = 0; j < octets[i].length(); ++j) {
//             char c = octets[i][j];
//             if (c < '0' || c > '9') return false;
//             val = val * 10 + (c - '0');
//         }

//         if (val < 0 || val > 255) return false;
//         parts[i] = static_cast<uint32_t>(val);
//     }
//     result = (atoi(octets[0].c_str()) << 24) | ( atoi(octets[1].c_str() )<< 16) | (atoi(octets[2].c_str()) << 8) | atoi(octets[3].c_str());
// }
