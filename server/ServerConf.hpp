#ifndef SERVER_CONF_HPP
#define SERVER_CONF_HPP
#include <string>
#include <vector>

typedef struct listenPair {

    std::string host;
    std::string port; 
    // std::string server name // if we want to hande lvirtual servers

    listenPair();
    listenPair(const std::string , const std::string );
}listenPair;

class ServerConf{
    private:
        std::vector<listenPair > listens; // [interface , port]

    public :
        // std::vector<int>& getPorts(){return ports;}
        ServerConf();
        ~ServerConf();
        const std::vector<listenPair>& getListens();
        void addListen(const listenPair&);
        void addListen(const std::string&, const std::string&);
};

#endif

/*
Config 
    vecror ServerConf
ServerConf 
    vector listens [interface port ]
    location 



*/