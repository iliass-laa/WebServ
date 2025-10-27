#include "../headers/webserver.hpp"
#include "checkTree.hpp"




checkTree::checkTree()
{
    // std::string allowedContexts[3] = {"http", "server" , "location"};
    // // std::string alloweddirectives[10] = {"server_name", "root" , "listen", "index","autoindex", "return", "client_max_body_size" };

    // std::vector <std::string>s ;
    // // s.push_back("C_server");
    // std::pair<std::string,std::map< std::string, std::vector <std::string>>> tmp;
    // std::pair<std::string,std::map< std::string, std::vector <std::string>>> tmp;

    // for (int i =0; i < 3 ;i++)
    // {
    //     // allowedContexts[i]
    // }


    // std::pair<std::string, std::vector <std::string>> tmp ("http", s);
    // ContextSource.insert(tmp);
}

int checkTree::finalChecking(BaseNode *root)
{
    // ContextNode *cnode;
    // DirectiveNode *dnode;

    // try
    // {
    //     cnode = dynamic_cast <ContextNode *>(root);
    //     if (cnode->type_Context != HttpContext)
    //         throw (ConfigFileError(""));
    // }
    // catch (std::exception e)
    // {
    //     std::cerr <<RED << e.what()  << DEF<<std::endl;
    //     return 1;
    // }
    (void)root;
    return 0;
}