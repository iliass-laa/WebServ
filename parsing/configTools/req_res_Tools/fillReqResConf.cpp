#include "../../headers/webserver.hpp"
#include "../../../req-res/response.hpp"


void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host)
{
    ContextNode *cNode;
    DirectiveNode *dNode;
    std::string port, serverName;
    serverName = host.substr(0, host.find(":"));
    port = host.substr(host.find(":")+ 1,host.length());


    cNode = findHttpContext(root);
    cNode = findServerContext(cNode, serverName,std::atoi(port.c_str()));
    cNode = findLocationContext(cNode, uri);
    for(int i = 0; i < cNode->nbrChilds; i++)
    {
        if (cNode->Childs[i]->typeNode == isDirective)
        {
            dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
            if (!dNode->key.compare("autoindex"))
                if (!dNode->value.back().compare("on"))
                    obj.setAutoIndex(true);
            if (!dNode->key.compare("root"))
                obj.setDirectoryPath(dNode->value.back());
            if (!dNode->key.compare("index"))
            {
                //bool hasIndex don't have a setter
                obj.setIndexFile(dNode->value); 
            }
        }
    }
}