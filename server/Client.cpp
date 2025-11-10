#include "Client.hpp"

Client::Client(int fd,BaseNode* cnf, Core& core)
    : root(cnf)   
    , theBase(core)            // initialize in same order as declaration
    , client_fd(fd)
    , connected(true)
    , reqBuff()
    , resBuff()
    , respoBuff()
    , resOffset(0)
    , responseSize(0)
    , state(WAITTING_FOR_REQUEST)
    , keepAlive(true)
    , Req()
    , isCGI(false)
    // Constructor implementation
{
    Req.headerParsed = false;
    Req.contentLength = 0;
    Req.isChunked = false;
    Req.headerEndPos = 0;
}

Client::~Client() {
    // Destructor implementation
}

Client::Client(const Client& other):theBase(other.theBase) {
    // Copy constructor implementation
    (void)other;
}

Client& Client::operator=(const Client& other) {
    // Copy assignment operator implementation
    if (this != &other) {
        // Perform deep copy
    }
    return *this;
}

int Client::getFd() const {
    return client_fd;
}

bool Client::isConnected() const {
    return connected;
}

bool Client::hasDataToRead() const 
{ 
    return !reqBuff.size();
}

bool Client::hasDataToWrite() const 
{ 
    return !resBuff.size(); 
}

bool Client::readData(){

    char buffer[BUFFER];
    ssize_t bytes = recv(client_fd, buffer, BUFFER, 0); //
    if (bytes > 0){ 

        reqBuff.insert(reqBuff.end(), buffer , buffer + bytes);
        int checkReq = -1;
        try
        {
            checkReq = handleRequest(root,reqBuff,respoBuff, Req); // keep-alive == COMPLETEDEF
            std::cout << CYAN <<"CLIENT :: Reding DATA (after HandelReq)\n"
                        <<"URI :"<< Req.uri 
                        <<"\ncl_fd :"<< client_fd
                        <<"\ncl_Addr :"<< this
                        << "\nHandel Req Return:"<< checkReq
                        << DEF << "\n";
            if (checkReq == CGI)
            {
                try 
                {
                    std::cout <<PINK <<">>>>CLIENT_FD  THAT GOT CGI:"<< this->client_fd<< "\n"<<DEF;
                    int newFd = cgi.handelCGI(root, Req);
                    theBase.addToEventLoop(newFd);
                    theBase.setCGI_FD(newFd, this);
                    isCGI = true; 
                    std::cout << BLUE<<"checkReq is CGI :\n" <<"NewFD :"<< newFd<< "\n"<<DEF;
                }
                catch (std::exception& e){
                std::cout << RED<<"CGI FAILED :"<< e.what()<< DEF << std::endl;
                }
                checkReq = COMPLETEDEF;
            }
            // std::cout << "return handel request " << checkReq << "  enenenen"  << std::endl;
        }catch (std::exception& e){
            std::cout << CYAN<< "return handel request " << checkReq << "  AFTER EXCEPTION \n"  <<DEF<< std::endl;
            std::cout << e.what() << "Driss " << std::endl;
        }
        if(COMPLETE == checkReq ){
            std::cout << CYAN <<" CLIENT ::>>>>>>>>>>>>>>>>> is ever he was here who knows" << std::endl;
            std::cout <<" >>>>>>>>>>>>>>>>> I AM ABOUT TO SET THE 'CONNECTED=FALSE' " << std::endl;
            std::cout  <<" >>>>>>>>>>>>>>>>>to this CLIENT :" << this << std::endl;
            std::cout  <<" >>>>>>>>>>>>>>>>>to this CLIENT fd:" << this->getFd() << DEF<< std::endl;
            connected = false; // Client disconnected
        }
        return checkReq; // keep alive 
    }
    else if (bytes < 0){
        connected = false; // Client disconnected
    }
    return false;   
}

// return should be void no use 
bool Client::writeData(){ // to vector<char> of write_data
    std::cout << CYAN << "CLIET ::i am about to send a RESPO to this CL:" << this 
            << "\nFD_CL : "<< this->getFd()
            <<"\nCL_STATE :" << getClientState()<< "\n";
    if(getClientState() == WAITTING_FOR_RESPONSE){
        state = SENDING_RESPONSE;
        clearReqStruct();
        resBuff.clear();
        resBuff = getResBuffer();
        responseSize = respoBuff.size();
    }
    std::cout << CYAN <<"CLIENT :iam STUCK HERE \n"; 
    printVecChar(respoBuff) ;
    std::cout << "\n\nThe string Form:>>>\t\t\t"<< PINK << resBuff << DEF<< std::endl;
    if(!resBuff.size() || !connected)
        return false;
    resOffset = BUFFER;
    if(responseSize > 0 && responseSize < BUFFER)
        resOffset =  responseSize;
    // memset()
    resOffset = send(client_fd, resBuff.c_str(), resBuff.length(),0);
    std::cout << CYAN << "CLIET ::RESPO to this CL:" << this 
            << ",is SENT\nFD_CL : "<< this->getFd() << DEF << "\n";
    //>> tillas was here : i have no idea why u send "resOffset" as len instead of  " resBuff.length()", sorry i commented ur line.
    // resOffset = send(client_fd, resBuff.c_str(), resOffset,0);
    if (resOffset > 0 )
        responseSize -= resOffset;
    if(responseSize <= 0){
        state = WAITTING_FOR_REQUEST;
        // std::cout << PINK << resBuff << DEF<< std::endl;
        respoBuff.clear();
    }
    return false;
}

void Client::queueResponse(const std::string &response)// based on what the responce is , will be sended as string
{
    // add the responce to client resBuff
    resBuff += response ;
}

std::string Client::getReadBuffer(){
    return std::string(reqBuff.begin(), reqBuff.end());
}

std::vector<char>& Client::getRespoBuffer(){
    return respoBuff;
}  

std::string Client::getResBuffer(){
    return std::string(respoBuff.begin(), respoBuff.end());
}  


void Client::setResponseSize(ssize_t size){
    responseSize = size;
}

void Client::setClientState(int state){
    this->state = state;
}

int Client::getClientState() const { return state;}

void Client::setKeepAlive(int val){keepAlive = val;}
int Client::getKeepAlive() const{return keepAlive;}

void Client::setReqStruct (struct HttpRequest& req){
    this->Req = req;
}

HttpRequest& Client::getReqStruct (){
    return Req;
}

void Client::clearVectReq(){
    reqBuff.clear();
}


void Client::clearVectRes(){
    resBuff.clear();
}

void Client::clearReqStruct(){
    Req.method.clear();
    Req.uri.clear();
    Req.version.clear();
    Req.headers.clear();
    Req.body.clear();
    Req.boundary.clear();
    Req.contentLength = 0;
    Req.headerEndPos = 0;
    Req.headerParsed = false;
}

