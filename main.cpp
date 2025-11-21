#include "parsing/headers/webserver.hpp"
#include "server/Core.hpp"

int main(int ac, char *av[])
{
    signal(SIGPIPE, SIG_IGN);
    std::string confPath("configfile.conf");
    BaseNode *root = NULL;
    if (ac == 2) 
        confPath = av[1];
    if  (ac > 2)
    {
        std::cerr <<RED << "Need Only One Valid Config File !"<< std::endl;
        return 1; 
    }
    try
    {
        root = parseConfigFile(confPath.c_str());
        if (!root)
            throw(ConfigFileError("Root is NULL"));
        Core magicSystem(root);
        magicSystem.run();   
    }
    catch(const std::exception& e)
    {
        if (root != NULL)
            freeTree(root);
        std::cerr << "**"<< e.what()  << "**"<< std::endl;
    }
    return 0;
}
