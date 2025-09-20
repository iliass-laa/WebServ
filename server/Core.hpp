#ifndef CORE_HPP
#define CORE_HPP
#include"Socket.hpp"
#include"Client.hpp"
#include"EventLoop.hpp"
#include<vector>

class Core {
    private:
        std::vector<Socket*> servers;
        std::map<int, Client*> clients;
        EventLoop event_loop;
        bool running;
        // Config obj;

        void handleSocketEvent(int fd, short events);
        bool isServerSocket(int fd);
        void handleNewConnection(int server_fd);
        void handleClientEvent(int client_fd, short events);
        void processClientRequest(Client *client);



    public:
        MultiServer();
        ~MultiServer();
        MultiServer(const MultiServer& other);
        MultiServer& operator=(const MultiServer& other);
        bool addServer(int port);
        void run();
        void stop();
};

#endif