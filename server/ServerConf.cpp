#include"ServerConf.hpp"

listenPair::listenPair():host("0.0.0.0"), port("8080"){}
listenPair::listenPair(const std::string host, const std::string port):host(host),port(port){}

ServerConf::ServerConf(){}

ServerConf::~ServerConf(){}

const std::vector<listenPair>& ServerConf::getListens(){
    return listens;
}

void ServerConf::addListen(const listenPair& newLsiten){
    listens.push_back(newLsiten);
}

void ServerConf::addListen(const std::string&host , const std::string& port){
    listens.push_back(listenPair(host,port));
}

