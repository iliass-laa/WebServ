#ifndef CORE_HPP
#define CORE_HPP
#include"Socket.hpp"
#include"Client.hpp"
#include"EventLoop.hpp"
#include"../parsing/headers/webserver.hpp"
#include<vector>
#include<set>
#include<map>
#include<iostream>
#include<string>


class ConfigFileError;
// #include"../parsing/headers/AST.hpp"
class BaseNode;
class Client;

class Core {
    private:
        std::vector<Socket*> servers;
        std::map<int, Client*> clients;
        std::map<int, Client*> cgi;
        EventLoop event_loop;
        bool running;
        BaseNode* root;
        std::set<std::string> pairs;

        void handleSocketEvent(int fd, short events);
        bool isServerSocket(int fd);
        void handleNewConnection(int server_fd);
        void handleClientEvent(int client_fd, short events);
        void processClientRequest(Client *client);
        void handleNewCgi(int fd_cgi, Client* cl);
        Client* isCgi(int fd_cgi);
        void handelCgiResponce(int, short,Client*);

        // void editSocket();
        
    public:
        Core();
        Core(BaseNode* );
        ~Core();
        Core(const Core& other);
        Core& operator=(const Core& other);
        bool addServers();
        void addToEventLoop(int newFd);
        // bool addServer(int port);
        void run();
        void stop();
        void setPairs(std::set<std::string>&);
        void setCGI_FD(int , Client *);
};

#endif