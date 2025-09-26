#ifndef CORE_HPP
#define CORE_HPP
#include"Socket.hpp"
#include"Client.hpp"
#include"EventLoop.hpp"
#include<vector>
#include<iostream>

class ServerConf{
    private:
        std::vector<int> ports;
        std::vector<std::string> ranges;

    public :
        std::vector<int>& getPorts(){return ports;}
};


class Config{
    private :
        std::vector<ServerConf* > servs;

    public:
        std::vector<ServerConf*> getServs(){
            return servs;
        }
};


class Core {
    private:
        std::vector<Socket*> servers;
        std::map<int, Client*> clients;
        EventLoop event_loop;
        bool running;
        Config obj;

        void handleSocketEvent(int fd, short events);
        bool isServerSocket(int fd);
        void handleNewConnection(int server_fd);
        void handleClientEvent(int client_fd, short events);
        void processClientRequest(Client *client);
        
    public:
        Core();
        ~Core();
        Core(const Core& other);
        Core& operator=(const Core& other);
        bool addServers(int port);
        void run();
        void stop();
};

#endif