#include"Core.hpp"
#include"../parsing/headers/webserver.hpp"

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
void Core::run(BaseNode* root){
    // Fill pairs here 
    // (void)root;
    fillServerConf(root, *this);
    if(!addServers()){
        return ;
    }
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
    if(isServerSocket(fd)){
        std::cout << "server socket " << fd << std::endl;
        handleNewConnection(fd);
    }
    else{
        std::cout << "client " << fd << std::endl;
        handleClientEvent(fd, events);
    }
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

    if(events & POLLIN ){
        if(client)
        client->readData();
    }

    // send response
    if(events & POLLOUT)
        client->writeData();

    // disconnect the client 
    if ( !(client->isConnected()) || (events & (POLLERR | POLLHUP))){
        event_loop.removeSocket(client_fd);
        delete client;
        clients.erase(it);
    }

    
}

// bool Core::addServer(int port){
//         // Socket* server = new Socket();

//         // if (!server->create() || !server->bind(port) ||
//         //     !server->listen(5) )
//         //     // || !server->setNonBlocking())
//         // {
//         //     std::cerr << "Failed to create server on port " << port << std::endl;
//         //     delete server;
//         //     return false;
//         // }

//         // // Add server socket to event loop
//         // event_loop.addSocket(server->getFd(), POLLIN);

//         // std::cout << "Server listening on port " << port << std::endl;
//         // servers.push_back(server);
//         return (ptrue);
// }

bool Core::addServers(){

    
    std::cout << "start add server " << pairs.size()  << std::endl; 
    printPairs(pairs);
    for(std::set<std::string>::iterator it = pairs.begin() ; it != pairs.end() ; it++ ){

        Socket* server = new Socket();
        
        std::cout << "addservers : inside loop" << std::endl;
        if (!server->create()
        || !server->setNonBlocking()
        || !server->bind(*it) 
        || server->listen(5) )
        {
            std::cerr << "00 Failed to create server on port " << *it << std::endl;
            delete server;
            return false;
        }

        // Add server socket to event loop
        event_loop.addSocket(server->getFd(), POLLIN);

        std::cout << "Server listening on port " << *it << std::endl;
        servers.push_back(server);
    }
    return true;
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
                    HandelCLientEvent
*/


void Core::setPairs(std::set<std::string>& param){
    pairs.insert(param.begin(), param.end());
}

