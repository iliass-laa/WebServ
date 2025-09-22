#include <string>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include"Socket.hpp"
#include"Core.hpp"

int main()
{

    std::cout << " WebServ A 3zzy " << std::endl ;
    Core c1;
    c1.run();
}

/*

    loop 
        declare fd_set ( read_set , write_set )
            (fd_set is a data structure used with the select() 
            system call for monitoring multiple file descriptors
            for I/O events. Let me explain it in detail)
        clear/initialize fd_set using macros FD_ZERO( & FD_SET )
        add servers socket fd to read_set
        add clients socket fd to write_set
            client can send and recv data
            * waintng for data -> add to read_set
            * ready to send data -> add to write_set
        call EPOLL / SELECT / POLL , and check for failure  [select clear unready fds] using poll ,upgrade version of select and widely supported
            [select return] == 0 => continue
        loop over servers for new conncection , using macro FD_ISSET()
        loop over clients for I/O , using macro FD_ISSET()
            write to socket (recv HTTP Request)


            read from socket (send HTTP Request)
*/