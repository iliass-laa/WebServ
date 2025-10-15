#include "Client.hpp"

Client::Client(int fd,BaseNode* cnf)
: root(cnf)               // initialize in same order as declaration
    , client_fd(fd)
    , connected(true)
    , reqBuff()
    , resBuff()
    , respoBuff()
    , resOffset(0)
    , responseSize(0)
    , keepAlive(true)
    // Constructor implementation
{}

Client::~Client() {
    // Destructor implementation
}

Client::Client(const Client& other) {
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

    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0); //

    if (bytes > 0){
        buffer[bytes] = '\0';
        std::vector<char> readed(buffer, buffer + bytes);
        reqBuff.clear();
        reqBuff.insert(reqBuff.begin(), readed.begin(), readed.end());

        std::cout << ">>>>>>>>>\t\t "<< GREEN ;
        for (std::vector<char>::iterator it = reqBuff.begin(); it != reqBuff.end() ; it++ ){
            std::cout   << *it ;
        }
        std::cout << DEF <<"<<<<<<<<<"<< std::endl;

        int checkReq = handleRequest(root,reqBuff,respoBuff); // keep-alive == COMPLETEDEF
        if(COMPLETE == checkReq ){
            std::cout << BLUE <<" >>>>>>>>>>>>>>>>> is ever he was here who knows" << DEF<< std::endl;
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
    if(getClientState() == WAITTING_FOR_RESPONSE){
        state = SENDING_RESPONSE;
        resBuff.clear();
        resBuff = getResBuffer();
        responseSize = respoBuff.size();
    }
    if(!resBuff.size() || !connected)
        return false;
    // std::cout << ">>>\t\t\t"<< PINK << resBuff << DEF<< std::endl;
    resOffset = BUFFER;
    if(responseSize > 0 && responseSize < BUFFER)
        resOffset =  responseSize;
    resOffset = send(client_fd, resBuff.c_str(), resOffset,0);
    if (resOffset > 0 )
        responseSize -= resOffset;
    if(responseSize <= 0){
        state = WAITTING_FOR_REQUEST;
        std::cout << PINK << resBuff << DEF<< std::endl;
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

void Client::clearVectReq(){
    reqBuff.clear();
}


void Client::clearVectRes(){
    resBuff.clear();
}

