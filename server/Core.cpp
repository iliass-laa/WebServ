#include"Core.hpp"

Core::Core(){}
Core::~Core(){}
Core::Core(const Core& obj){
    (void)obj;
}
Core& Core::operator=(const Core& obj){
    (void)obj;
    return *this;
}

// run 
// stop



/*  
    algo

    * add servers (form config to creating sockets)
    * run webserv 
        - while true 
            . get ready events (poll)
            . handelSocketEvent( if else )
                isServerSocket
                    handelNewConnection
                isClientEvent
                    handelClientEvent


*/