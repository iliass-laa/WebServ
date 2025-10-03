#include"ServerConf.hpp"

listenPair::listenPair():host("0.0.0.0"), port("8080"){}
listenPair::listenPair(const std::string host, const std::string port):host(host),port(port){}

ServerConf::ServerConf(){}
ServerConf::ServerConf(std::string host, std::string port){
    listens.push_back(listenPair(host,port));
}

ServerConf::ServerConf(std::string host, std::string port){
    listens.push_back(listenPair(host,port));
}

ServerConf::ServerConf( std::vector<listenPair>& pairs){
    listens.insert(listens.end(), pairs.begin(), pairs.end());
}

ServerConf::~ServerConf(){}

const std::vector<listenPair>& ServerConf::getListens(){
    return listens;
}

void ServerConf::addListen( listenPair& newLsiten){
    listens.push_back(newLsiten);
}

void ServerConf::addListen( std::string&host ,  std::string& port){
    listens.push_back(listenPair(host,port));
}

void ServerConf::addListens(std::vector<listenPair>& pairs){
    listens.insert(listens.end(), pairs.begin(), pairs.end());
}

