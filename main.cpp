#include "parsing/headers/webserver.hpp"
#include "server/Core.hpp"

int main(int ac, char *av[])
{
    // signal(SIGPIPE, SIG_IGN);
    std::string confPath("configfile.conf");
    BaseNode *root;
    if (ac == 2) 
        confPath = av[1];
    // try
    // {
        root = parseConfigFile(confPath.c_str());
        // printTree(root, 0);
        Core magicSystem(root);
        magicSystem.run();   
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what()  << "**"<< std::endl;
    // }
    return 0;
}
