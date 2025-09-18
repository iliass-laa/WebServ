#ifndef CORE_HPP
#define CORE_HPP
#include"Socket.hpp"
#include<vector>

class Core {
    private :
        std::vector<Socket*> servers;
        // std::map<int, Client*> clients;
        // EventLoop loop ;
        // bool riunning;
    public :
        Core();
        ~Core();
        Core(const Core&);
        Core& operator=(const Core&);

    private :
        

};

#endif