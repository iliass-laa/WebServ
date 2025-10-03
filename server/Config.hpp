#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerConf.hpp"


class Config{
    private :
        std::vector<ServerConf* > servs;

    public:
        Config();
        ~Config();
        std::vector<ServerConf*> getServs();
        void addServerConf(const std::vector<ServerConf*>);
        void addServerConf( ServerConf*);
};

#endif