#include "../headers/webserver.hpp"


class checkTree 
{
    // int allC, allD;
    std::map <std::string, std::map <std::string , std::vector <std::string > > > ContextSource;
    // std::map <std::string, std::map <std::string , std::vector <std::string > > > DirectiSource;
    // std::map <std::string, std::vector <std::string>> DirectiSource;

    int     checkContext(BaseNode *Context);
    int     checkDirective(BaseNode *Directive);

    public :
        checkTree();

        int finalChecking(BaseNode *root);
};