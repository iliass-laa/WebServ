#ifndef CORE_HPP
#define CORE_HPP
#include"Socket.hpp"
#include"Client.hpp"
#include"EventLoop.hpp"
<<<<<<< HEAD
=======
#include"../parsing/headers/webserver.hpp"
>>>>>>> fe7d159c647a0bb7bd9187ad2d11b74569ecca9c
#include<vector>
#include<set>
#include<map>
#include<iostream>
#include<string>
<<<<<<< HEAD
=======


class ConfigFileError;
>>>>>>> fe7d159c647a0bb7bd9187ad2d11b74569ecca9c
// #include"../parsing/headers/AST.hpp"
class BaseNode;
class Client;

class Core {
    private:
        std::vector<Socket*> servers;
        std::map<int, Client*> clients;
        EventLoop event_loop;
        bool running;
        BaseNode* root;
        std::set<std::string> pairs;

        void handleSocketEvent(int fd, short events);
        bool isServerSocket(int fd);
        void handleNewConnection(int server_fd);
        void handleClientEvent(int client_fd, short events);
        void processClientRequest(Client *client);
        // void editSocket();
        
    public:
        Core();
        Core(BaseNode* );
        ~Core();
        Core(const Core& other);
        Core& operator=(const Core& other);
        bool addServers();
        // bool addServer(int port);
        void run();
        void stop();
        void setPairs(std::set<std::string>&);
};

#endif