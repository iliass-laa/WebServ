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
    
    cNode = findLocationContext(cNode, "/");
    if (cNode)
    {
         for(int i = 0; i < cNode->nbrChilds; i++)
        {
            if (cNode->Childs[i]->typeNode == isDirective)
            {
                dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
                if (!dNode->key.compare("root"))
                    obj.setRoot(dNode->value.back());
                if (!dNode->key.compare("index"))
                        obj.setIndexFile(dNode->value);
            }
        }
    }
    cNode = findLocationContext(cNode, uri);
    if (cNode)
    {
        if (uri.find("/upload/") == 0)
            obj.setUploadSupport(true);
        for(int i = 0; i < cNode->nbrChilds; i++)
        {
            if (cNode->Childs[i]->typeNode == isDirective)
            {
                dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
                if (!dNode->key.compare("autoindex"))
                    if (!dNode->value.back().compare("on"))
                        obj.setAutoIndex(true);
                if (!dNode->key.compare("root"))
                    obj.setRoot(dNode->value.back());
                if (!dNode->key.compare("index"))
                        obj.setIndexFile(dNode->value);
            }
        }
    }
}