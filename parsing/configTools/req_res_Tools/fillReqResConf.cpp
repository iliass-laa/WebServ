#include "../../headers/webserver.hpp"
#include "../../../req-res/response.hpp"

void printThingsToCheck(DirectoryListing &obj)
{
    std::cout   << GREEN 
                << "root :" << obj.getRoot() 
                << "\nAutoIndex :" << obj.getAutoIndex()
                << "\nDefault ?: " << obj.getDefault()
                << "\nMax BodySize :" << obj.getMaxBodySize()
                << "\nHas Index File:"<< obj.getHasIndexFile()
                << DEF << "\n";
}

void checkDirectiveChilds(ContextNode *cNode, DirectoryListing &obj)
{
    DirectiveNode *dNode;
    std::vector <std::string> allowMeth;
    // std::string co[6] = {"MainContext","EventsContext", "HttpContext", "TypesContext", "ServerContext", "LocationContext" };

    // std::cout << RED << "Checking Directive for this Context:" <<co[cNode->type_Context]
    //             <<DEF << "\n";
    for (int i = 0;i < cNode->nbrChilds;i++)
    {
        if (cNode->Childs[i]->typeNode == isDirective)
        {
            dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
            if (!dNode->key.compare("autoindex")&& !dNode->value.back().compare("on"))
                obj.setAutoIndex(true);
            else if (!dNode->key.compare("index")&& dNode->value.size() != 0)
                obj.setIndexFile(dNode->value);
            else if (!dNode->key.compare("root"))
                obj.setRoot(dNode->value.back());
            else if (!dNode->key.compare("limit_except"))
                obj.setAllowedMethods(dNode->value);
        }
    }
    std::cout << "Root :" << obj.getRoot() << "\n";
}

void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host)
{
    ContextNode *cNode, *serverNode, *httpNode;
    std::string port, serverName;
    std::vector <std::string> allowMethDef ;
    allowMethDef.push_back("GET");
    allowMethDef.push_back("POST");
    allowMethDef.push_back("DELETE");
    obj.setAllowedMethods(allowMethDef);

    std::cout <<PINK << "URI :"<<uri
                << ",HOST : "<< host
                <<"\n"<<DEF ;

    int iport;
    serverName = host.substr(0, host.find(":"));
    port = host.substr(host.find(":")+ 1,host.length());


    httpNode = findHttpContext(root);
    if(!httpNode)
        throw(ConfigFileError("No http Found"));
    iport = std::atoi(port.c_str());
    if (iport < 0)
        throw(ConfigFileError("invalid Port"));
    serverNode = findServerContext(httpNode, serverName,iport);
    if(!serverNode)
        throw(ConfigFileError("No Server Found"));
    checkDirectiveChilds(serverNode, obj);
    cNode = findLocationContext(serverNode, uri);
    if (!cNode)
        throw(ConfigFileError("No Location Found"));
    std::cout <<PINK << "VAL of Context:"<<cNode->val.back()<<"\n"<<DEF ;
    if (cNode->val.back().compare("/") == 0)
        obj.setDefault(true);
    checkDirectiveChilds(cNode, obj);
    printThingsToCheck(obj);
 }