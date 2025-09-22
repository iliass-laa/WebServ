#include"Core.hpp"

Core::Core():running(false){}
Core::~Core(){}
Core::Core(const Core& obj){
    (void)obj;
}
Core& Core::operator=(const Core& obj){
    (void)obj;
    return *this;
}

// stop
void Core::stop(){
    running = false;
} 

// run 
void Core::run(){
    running = true ;
    while (running){
        std::vector<std::pair<int,short> > readyEvents = event_loop.waitForEvents(0); 
        // time out here is 0 => non-blocking poll()
        for(std::vector<std::pair<int,short> >::iterator it = readyEvents.begin() ; 
        it != readyEvents.end() ;
        it++
        ){
            handleSocketEvent(it->first , it->second);
        }
    }
}   

void Core::handleSocketEvent(int fd, short events){
    if(isServerSocket(fd))
        handleNewConnection(fd);
    else
        handleClientEvent(fd, events);
}

bool Core::isServerSocket(int fd){
    for(std::vector<Socket*>::iterator it = servers.begin();
    it != servers.end();
    it++
    ){
        if((*it)->getFd() == fd)
            return true;
    }
    return false;
}

void Core::handleNewConnection(int server_fd){
    // retrive server where event occured
    Socket* server = NULL;

    for( std::vector<Socket*>::iterator it = servers.begin();
        it != servers.end();
        it++
    ){
        if((*it)->getFd() == server_fd ){
            server = *it ;
            break;       
        }
    }

    if(!server)
        return ;

    // accept new connection 
    int new_client = server->accept();
    if(new_client != -1){
        Client* cl = new Client(new_client);
        event_loop.addSocket(new_client, POLLIN);
        clients[new_client] = cl;
        std::cout << "new client connected !!" << std::endl ;
    }
    return ;
}

void Core::handleClientEvent(int client_fd, short events){
    // retrive client from map 
    std::map<int, Client*>::iterator it = clients.find(client_fd) ;
    if(it == clients.end())
        return ;
    Client* client = it->second;

    // read request
    if(events & POLLOUT)
        client->writeData();

    // disconnect the client 
    if ( !(client->isConnected()) || (events & (POLLERR | POLLHUP))){
        event_loop.removeSocket(client_fd);
        delete client;
        clients.erase(it);
    }

    
}
/*  
    algo

    * add servers (form config to creating sockets)
    * run webserv 
        - bool running is true now
        - while true 
            . get ready events (poll)
            . handelSocketEvent( if else )
                isServerSocket
                    handelNewConnection
                isClientEvent
                    handelClientEvent


*/