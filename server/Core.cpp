#include"Core.hpp"
#include"../parsing/headers/webserver.hpp"


void printVecChar( std::vector<char> Vec)
{
    int i = 0;
    std::vector<char>:: iterator it = Vec.begin(); 
    while (it != Vec.end())
    {
        if (i == 1000)
        {
            std::cout << "....\n" ; 
            break;
        }
        std::cout << *it ; 
        it++;
        i++;
    }
}

// ORTHODOX FORM
Core::Core():running(false),root(NULL){}

Core::Core(BaseNode* obj)
    :sessionMaster(Session(300))
    ,running(false) 
    ,root(obj) 
    {}
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
   
        try{
            fillServerConf(root, *this);
        }catch(ConfigFileError& e){
            std::cout << e.what() << std::endl;
            freeTree(root);
        }
        if(!addServers()){
            return ;
        }
        running = true ;
        while (running){
            std::vector<std::pair<int,short> > readyEvents = event_loop.waitForEvents(0); 
            // time out here is 0 => non-blocking poll()
            for(std::vector<std::pair<int,short> >::iterator it = readyEvents.begin() ; it != readyEvents.end() ;it++ )
                handleSocketEvent(it->first , it->second);
        }

}   


//*****************HANDEL**SOCKET**EVENT******************************************************************
void Core::handleSocketEvent(int fd, short events){
    Client* cl = NULL;
    if(isServerSocket(fd)){
        std::cout << "[server socket] " << fd << std::endl;
        handleNewConnection(fd);
    }
    else if((cl = isCgi(fd))){
        handelCgiResponce(fd, events,cl);
    }
    else {
        // std::cout << "client " << fd << std::endl;
        handleClientEvent(fd, events);
    }
}

void    Core::handelCgiResponce(int fd, short events, Client* client){

    if (!(events & POLLIN))
        return;
    std::cout << PINK << "handelCgiResponce><<< \n"
            << "Fd i read from AS CGI:" << fd <<"\n"
            << "events :" <<( events )<<"\n"
            << "clFD:" << client->getFd()<<"\n";
   
    int genresp = client->getCGI().generateResponse(fd, client->getRespoBuffer()); 
    if (genresp == 1)
    {
        std::cout << "here >>>>>>\n";
        return ;
    }
    client->setRequestReaded(true);
    event_loop.updateSocketEvents(client->getFd() ,POLLOUT );
    // check the Vec char for the gen SID (set-Cookie) and add it to session ids map
    findSID(client);

    std::cout << YELLOW<< ".......................CGI ::AFTER WRITING DATA  \n"<< DEF;
    event_loop.removeSocket(fd);
    std::map<int, Client*> :: iterator it  = cgi.find(fd) ;
    if (it != cgi.end())
        cgi.erase(it);
    client->setIsCGI(false);

    // client->setRequestReaded(true);
    // event_loop.updateSocketEvents(client->getFd() ,POLLOUT );
    // client->clearVectReq();
    // if(client->getClientState() == WAITTING_FOR_REQUEST){
    // event_loop.updateSocketEvents(client->getFd() ,POLLIN );
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
        Client* cl = new Client(new_client, root, *this);
        event_loop.addSocket(new_client, POLLIN);
        clients[new_client] = cl;
        std::cout << "\t\t[new client connected] [" << new_client << "] !!" << std::endl ;
    }
    return ;
}

void Core::setCGI_FD(int fd, Client *cl){
    std::pair <int, Client *> p (fd, cl);
    cgi.insert(p);
}

// void Core::handleNewCgi(int fd_cgi, Client* clt){
//     std::pair<int , Client*> cc(fd_cgi, clt);
//     cgi.insert(cc);
//     event_loop.addSocket(fd_cgi, POLLIN);
//     std::cout << "\t\t[new cgi connected] [" << fd_cgi << "] !!" << std::endl ;
//     return ;
// }

Client* Core::isCgi(int fd_cgi){
    std::map<int , Client*>::iterator it = cgi.find(fd_cgi);
    if(it != cgi.end())
        return it->second;
    return NULL;
}

//***************HANDEL**CLIENT**EVENT********************************************************************
void Core::handleClientEvent(int client_fd, short events){
    std::map<int, Client*>::iterator it = clients.find(client_fd) ;
    if(it == clients.end()){
        std::cout << "[HandelClientEvent][No Client Found]" << std::endl;
        return ;
    }
    Client* client = it->second;
    
    if( events & POLLIN ){
        int readDataState = client->readData(); 
        if( readDataState){
            client->setRequestReaded(true);
            event_loop.updateSocketEvents(client->getFd() ,POLLOUT );
            client->clearVectReq();
        }
    }
    
    // send response
    else if(events & POLLOUT ){
        std::cout << RED << "alololololo------------------------" ;
        printVecChar(client->getRespoBuffer() );
        std::cout << "alololololo------------------------" << DEF;
        if(client->writeData()){
            event_loop.updateSocketEvents(client->getFd() ,POLLIN );
        }
    }
    
    // disconnect the client 
    if ( !(client->isConnected())){
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
            freeTree(root);
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



void Core::addToEventLoop(int newFd)
{
    event_loop.addSocket(newFd, POLLIN);
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

/************************************************** */
/**************************SESSION***************** */
/************************************************** */

bool Core::checkSession(std::string sid)
{
    Session_t* sess = sessionMaster.lookup_session(sid);
    if(NULL == sess)
        return false;
    return true;
}


void Core::findSID(Client* cl){
    std::cout <<  "herererererererer" << std::endl;
      printVecChar(cl->getRespoBuffer()) ;
    std::cout <<  "herererererererer" << std::endl;
    std::string tmpRespo = cl->getresBuffStringer();
    std::string::size_type pos ;
    std::string token("session-id=");
    if( (pos= tmpRespo.find(token )) != std::string::npos ){
        std::string::size_type lastSid;
        if( (lastSid = tmpRespo.find_first_of(pos + token.size() ,',')) != std::string::npos){
            std::string sidCookie = tmpRespo.substr(pos + token.size() , (lastSid - pos + token.size()));
            std::cout << "sid >>>>"  << sidCookie << std::endl;
            sessionMaster.create_session(sidCookie);
        }
    }
}