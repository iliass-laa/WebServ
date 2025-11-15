#include "Client.hpp"

Client::Client(int fd,BaseNode* cnf, Core& core)
    : root(cnf)   
    , theBase(core)            // initialize in same order as declaration
    , client_fd(fd)
    , connected(true)
    , reqBuff()
    , resBuffString()
    , respoBuff()
    , resOffset(0)
    , Req()
    , isCGI(false)
    , requestReaded(false)
    , respoSize(0)
    , respoSended(0)
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

bool Client::readData(){

    char buffer[BUFFER];
    ssize_t bytes = recv(client_fd, buffer, BUFFER, 0); //
    if (bytes > 0){ 

        reqBuff.insert(reqBuff.end(), buffer , buffer + bytes);
        int checkReq = -1;
        try
        {
            checkReq = handleRequest(root,reqBuff,respoBuff, Req); // keep-alive == COMPLETEDEF
            if (checkReq == CGI)
            {
                respoBuff.clear();
                try 
                {
                    int newFd = cgi.handelCGI(root, Req);
                    if (newFd)
                    {
                        theBase.addToEventLoop(newFd);
                        theBase.setCGI_FD(newFd, this);
                        isCGI = true; 
                    }
                    else
                    {
                        respoBuff.clear();
                        respoBuff =  buildErrorResponse(403); 
                        return true;
                    }   
                }
                catch (std::exception& e){
                std::cout << RED<<"CGI FAILED :"<< e.what()<< DEF << std::endl;
                }
                clearVectReq();
                return false;
            }
        }catch (std::exception& e){
            std::cout << CYAN<< "return handel request " << checkReq << "  AFTER EXCEPTION \n"  <<DEF<< std::endl;
            std::cout << e.what() << "Driss " << std::endl;
        }
        if(checkReq == COMPLETE || checkReq == COMPLETEDEF)
            return true;
        return false;
    }
    else if (bytes <= 0){
        std::cout << "bytes readed " << bytes << std::endl;
        // if (!isCGI)
        connected = false;
    }
    return true;   
}


// return should be void no use 
bool Client::writeData(){
// to vector<char> of write_data
    if(requestReaded){
        std::cout << CYAN << "Client ::About to clear Things\n"<<DEF;
        clearReqStruct();
        resBuffString.clear();
        resBuffString = getresBuffStringer();
        // std::cout << BLUE <<  "requestReaded is True and the BuffStr of responce is getting filled\n" <<DEF;
        requestReaded = false;
        respoSize = respoBuff.size();
        respoSended = 0;
    }
    if(!respoSize)
        return true; // nothing to send
    resOffset = BUFFER;
    if(respoSize > 0 && respoSize < BUFFER)
        resOffset = respoSize ;
    if (respoSize - respoSended < BUFFER)
        resOffset = respoSize - respoSended;

   
    const char *msg = resBuffString.c_str() + respoSended;
    resOffset = send(client_fd, msg, resOffset,0);
    respoSended += resOffset;

    if(resOffset <= 0 ||  respoSended == respoSize ){
        respoBuff.clear();
        requestReaded = false;
        return true;
    }
    return false;
}

std::vector<char>& Client::getRespoBuffer(){
    return respoBuff;
}  

std::string Client::getresBuffStringer(){
    return std::string(respoBuff.begin(), respoBuff.end());
}  


void Client::setRequestReaded(bool val){
    this->requestReaded = val;
}

void Client::setClientState(int state){
    this->state = state;
}

int Client::getClientState() const { return state;}

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
    resBuffString.clear();
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

