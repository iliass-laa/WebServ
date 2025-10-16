#include "../../headers/webserver.hpp"
#include "../../../req-res/response.hpp"

void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host)
{
    ContextNode *cNode, *serverNode, *httpNode;
    DirectiveNode *dNode;
    std::string port, serverName;
    std::vector <std::string> allowMeth ({"GET", "POST", "DELETE"});
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
    cNode = findLocationContext(serverNode, uri);
    if (!cNode)
        throw(ConfigFileError("No Location Found"));
    // shere <<PINK << "VAL of Context:"<<cNode->val.back()<<"\n"<<DEF ;
    if (cNode->val.back().compare("/") == 0)
        obj.setDefault(true);
    obj.setAllowedMethods(allowMeth);
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
        }
    }
}