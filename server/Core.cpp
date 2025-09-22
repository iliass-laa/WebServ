#include"Core.hpp"
#include"sstream"
#include<fstream>

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

    if(events & POLLIN){
        if(client->readData() && client->hasDataToRead())// use parse func here to check is there data to read
            processClientRequest(client);
    }

    // read request
    if(events & POLLOUT)
        client->writeData();

    // disconnect the client 
    if ( !(client->isConnected()) || (events & (POLLERR | POLLHUP))){
        event_loop.removeSocket(client_fd);
        delete client;
        clients.erase(it);
    }
    // else{
    //     short new_events = POLLIN;
    //     if (client->hasDataToWrite()) // find why
    //     {
    //         new_events |= POLLOUT;
    //     }
    //     event_loop.updateSocketEvents(client_fd, new_events);
    // }
}


void Core::processClientRequest(Client *client)
{
    std::string request = client->getReadBuffer();

    // Simple HTTP response
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 4215\r\n"
        "\r\n";
        // "Hello, World!";
    std::ifstream file("index.html", std::ios::in | std::ios::binary);
    if (!file) {
        throw std::ios_base::failure("Failed to open file: index.html");
    }

    std::stringstream content;
    content << file.rdbuf();
    response += content.str();
    client->queueResponse(response);
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

/* HandelCLientEvent

*/

bool Core::addServers(int port){
    std::vector<ServerConf*> servs = obj.getServs();
    for ( std::vector<ServerConf*>::iterator it = servs.begin();
        it != servs.end();
        it++

    ){
        Socket* server = new Socket();

        if (!server->create() || !server->bind(port) ||
            !server->listen(5) || !server->setNonBlocking())
        {
            std::cerr << "Failed to create server on port " << port << std::endl;
            delete server;
            return false;
        }

        // Add server socket to event loop
        event_loop.addSocket(server->getFd(), POLLIN);

        std::cout << "Server listening on port " << port << std::endl;
        servers.push_back(server);
    }
    return true;
}
