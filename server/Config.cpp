#include"Config.hpp"

Config::Config(){}

Config::~Config(){
    for (size_t i = 0; i < servs.size(); i++) {
        delete servs[i];  // Free each object
    }
    servs.clear(); 
}

std::vector<ServerConf*> Config::getServs(){

    return servs;
}

void Config::addServerConf(const std::vector<listenPair> range){
    servs.insert(servs.end(),range.begin(), range.end() );
}

void Config::addServerConf( ServerConf* newServerConf){
    servs.push_back(newServerConf);
}