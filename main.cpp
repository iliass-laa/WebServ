#include "parsing/headers/webserver.hpp"
#include "server/Core.hpp"

int main(int ac, char *av[])
{
    std::string confPath("configfile.conf");
    BaseNode *root;
    if (ac == 2) 
        confPath = av[1];
    try
    {
        root = parseConfigFile(confPath.c_str());
        Core magicSystem;
        magicSystem.run(root);   
        // (void)root;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
