#ifndef CORE_HPP
#define CORE_HPP
#include"Socket.hpp"
#include"Client.hpp"
#include"EventLoop.hpp"
#include<vector>
#include<set>
#include<iostream>
#include<string>
#include"../parsing/headers/AST.hpp"




class Core {
    private:
        std::vector<Socket*> servers;
        std::map<int, Client*> clients;
        EventLoop event_loop;
        bool running;
        // Config obj;
        // std::list<pair<std::> >
        std::set<std::string> pairs;

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
        bool addServers();
        // bool addServer(int port);
        void run(BaseNode*);
        void stop();
        void setPairs(std::set<std::string>&);
};

#endif