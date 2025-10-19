#include"Core.hpp"
#include"../parsing/headers/webserver.hpp"


// ORTHODOX FORM
Core::Core():running(false),root(NULL){}

Core::Core(BaseNode* obj):running(false), root(obj){}
 
Core::~Core(){}
Core::Core(const Core& obj){
    (void)obj;
}
Core& Core::operator=(const Core& obj){
    (void)obj;
    return *this;
}




//********************STOP******************************************************************************
void Core::stop(){
    running = false;
} 

//********************RUN************************ *******************************************************
void Core::run(){
   

        fillServerConf(root, *this);
        if(!addServers()){
            return ;
        }
        running = true ;
        while (running){
            std::vector<std::pair<int,short> > readyEvents = event_loop.waitForEvents(-1); 
            // time out here is 0 => non-blocking poll()
            for(std::vector<std::pair<int,short> >::iterator it = readyEvents.begin() ; it != readyEvents.end() ;it++ )
            handleSocketEvent(it->first , it->second);
        }

}   


//*****************HANDEL**SOCKET**EVENT******************************************************************
void Core::handleSocketEvent(int fd, short events){
    if(isServerSocket(fd)){
        std::cout << "[server socket] " << fd << std::endl;
        handleNewConnection(fd);
    }
    else{
        // std::cout << "client " << fd << std::endl;
        handleClientEvent(fd, events);
    }
}




//***********IS**SERVER**SOCKET***************************************************************************
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


//****************HANDEL**NEW**CONNECION*******************************************************************
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
        Client* cl = new Client(new_client, root);
        event_loop.addSocket(new_client, POLLIN);
        clients[new_client] = cl;
        std::cout << "\t\t[new client connected] [" << new_client << "] !!" << std::endl ;
    }
    return ;
}



//***************HANDEL**CLIENT**EVENT********************************************************************
void Core::handleClientEvent(int client_fd, short events){
    // retrive client from map 
    std::map<int, Client*>::iterator it = clients.find(client_fd) ;
    if(it == clients.end()){
        std::cout << "[HandelClientEvent][No Client Found]" << std::endl;
        return ;
    }
    Client* client = it->second;
    
    if( events & POLLIN ){
        client->setClientState(READING_REQUEST);
        int readDataState = WAITTING;
        if(client->getClientState() == READING_REQUEST)
            readDataState = client->readData(); 
        if( readDataState == COMPLETE || readDataState == COMPLETEDEF){
            event_loop.updateSocketEvents(client->getFd() ,POLLOUT );
            client->setClientState(WAITTING_FOR_RESPONSE);
            client->clearVectReq();
        }
    }
    
    // send response
    else if(events & POLLOUT ){
        // std::cout << " **** Sending Response ****"<< std::endl;
        if(client->getClientState() == WAITTING_FOR_RESPONSE || client->getClientState() == SENDING_RESPONSE )
            client->writeData();
        if(client->getClientState() == WAITTING_FOR_REQUEST){
            event_loop.updateSocketEvents(client->getFd() ,POLLIN );

        }

    }
    
    // disconnect the client 
    // if ( !(client->isConnected()) || (events & (POLLERR | POLLHUP))){
    if ( !(client->isConnected())){
        std::cout << RED << "client"<< client->getFd() <<  " lose tcp connection !!!" << DEF << std::endl;
        event_loop.removeSocket(client_fd);
        delete client;
        clients.erase(it);
    }
}

//*************ADD**SERVERS******************************************************************************
bool Core::addServers(){    
    for(std::set<std::string>::iterator it = pairs.begin() ; it != pairs.end() ; it++ ){
        Socket* server = new Socket();
        if (!server->create()
        || !server->setNonBlocking()
        || !server->bind(*it) 
        || server->listen(5) 
        )
        {
            std::cerr << "[server][addServers] Failed to create server on port " << *it << std::endl;
            delete server;
            return false;
        }

        // Add server socket to event loop
        event_loop.addSocket(server->getFd(), POLLIN);

        std::cout << "[server][addServers] Server listening on ip:port " << *it << std::endl;
        servers.push_back(server);
    }
    return true;
}


//****************SET**PAIRS********************************************************************************
void Core::setPairs(std::set<std::string>& param){
    pairs.insert(param.begin(), param.end());
}


// void Core::editSocket(client){

// }
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


