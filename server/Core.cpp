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

    std::cout << YELLOW<<"CGI ::AFTER WRITING DATA  \n"<< DEF;
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
int nTime;
int i = 0;
void Core::handleClientEvent(int client_fd, short events){
    // std::cout << RED << "Client :" << client_fd 
    //             << "\n for the " <<nTime << " Times\n"
    //             << "Event ::" <<events << "\n"<<DEF;
    nTime++;
    if (nTime == 500)
        exit(22);
    std::string stt[6] = {"READING_REQUEST", // 0
    "WAITTING_FOR_REQUEST", // 2
    "SENDING_RESPONSE", // 1
    "WAITTING_FOR_RESPONSE", // 2
    "INACTIVE",
    "WAITTING"};
    // retrive client from map 
    std::map<int, Client*>::iterator it = clients.find(client_fd) ;
    if(it == clients.end()){
        std::cout << "[HandelClientEvent][No Client Found]" << std::endl;
        return ;
    }
    Client* client = it->second;
    
    if( events & POLLIN ){
        i++;
        int readDataState = client->readData(); 
        std::cout <<  PINK << "readState " << readDataState << " times " <<  i <<DEF << std::endl;
        if( readDataState){
            client->setRequestReaded(true);
            // std::cout << RED << ">>>>>>>>>>>>>>\n"<<  client->getresBuffStringer() <<  DEF << std::endl; 
            std::cout << RED << ">>>>>>>>>>>>>>\n"<<  client->getRequest() <<  DEF << std::endl; 
            event_loop.updateSocketEvents(client->getFd() ,POLLOUT );
            client->clearVectReq();
        }
    }
    
    // send response
    else if(events & POLLOUT ){
        if(client->writeData())
            event_loop.updateSocketEvents(client->getFd() ,POLLIN );
    }
    
    // disconnect the client 
    // if ( !(client->isConnected()) || (events & (POLLERR | POLLHUP))){
    if ( !(client->isConnected())){
        // std::cout << GREEN << "CORE :: \n"
        //     <<"SEEMS like this client is"<< RED<<" Leaving Us  "
        //     << GREEN<<":\n"
        //     <<"CL Addr:" << client
        //     << "\nCL fd :" <<client->getFd() << "\n"<<DEF;
        // std::cout << RED << "client"<< client->getFd() <<  " lose tcp connection !!!" << DEF << std::endl;
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

bool Core::updateSession(std::string sid, std::map<std::string, std::string>& cookies)
{
    Session_t* sess = sessionMaster.lookup_session(sid);
    if(NULL == sess){
        return false;
    }else{
        // update session
        (void)cookies;
        // std::string newSid = regenerate_session(sid);
    }
    return false;
}


bool Core::newSession( std::map<std::string, std::string>& cookies)
{
    // new session
    std::string newSid ;
    if( (newSid = sessionMaster.create_session(cookies)).size() != 0 ) { // set up session varaiable 
        std::pair<std::string, std::string > sid("SID", newSid);
        cookies.insert(cookies.begin() , sid);
        return true;
    }else{
        cookies.clear();
        return false;
    }
}
