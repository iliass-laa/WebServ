#include "../../headers/webserver.hpp"
#include "../../../req-res/response.hpp"
// #include "../../../req-res/request.hpp"


// void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host)
// {
//     ContextNode *cNode, *serverNode;
//     DirectiveNode *dNode;
//     std::string port, serverName;
//     serverName = host.substr(0, host.find(":"));
//     port = host.substr(host.find(":")+ 1,host.length());


//     cNode = findHttpContext(root);
//     if (!cNode)
//     if (!cNode)
//     {
//         std::cout << PINK<<"Http Not Found\n"<< DEF;
//         return ;
//     }
//     std::cout << ",hostName:"+ host+",uri :"+uri+"\n";
//     std::cout << "port:"+port+"\n";
//     std::cout << "serverName:"+serverName+"\n";
//     serverNode = findServerContext(cNode, serverName,std::atoi(port.c_str()));
//     if (!serverNode)
//     {
//         std::cout << PINK<<"Server Not Found\n"<< DEF;
//         return ;
//     }
    
//     cNode = findLocationContext(serverNode, "/");
//     if (cNode)
//     {
//          for(int i = 0; i < cNode->nbrChilds; i++)
//         {
//             if (cNode->Childs[i]->typeNode == isDirective)
//             {
//                 dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
//                 if (!dNode->key.compare("root"))
//                     obj.setRoot(dNode->value.back());
//                 if (!dNode->key.compare("index"))
//                         obj.setIndexFile(dNode->value);
//             }
//         }
//     }
//     std::cout <<BLUE + uri + "\n" ; 
//     cNode = findLocationContext(serverNode, uri);

//     if (cNode)
//     {
//         if (uri.find("/upload/") == 0)
//             obj.setUploadSupport(true);
//         for(int i = 0; i < cNode->nbrChilds; i++)
//         {
//             if (cNode->Childs[i]->typeNode == isDirective)
//             {
//                 dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
//                 if (!dNode->key.compare("autoindex"))
//                     if (!dNode->value.back().compare("on"))
//                         obj.setAutoIndex(true);
//                 if (!dNode->key.compare("root"))
//                     obj.setRoot(dNode->value.back());
//                 if (!dNode->key.compare("index"))
//                         obj.setIndexFile(dNode->value);
//             }
//         }
//     }
//     else
//     {
//         std::cout << "alooo\n";
//     }
// }


void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host)
{
    ContextNode *cNode, *serverNode, *httpNode;
    DirectiveNode *dNode;
    std::string port, serverName;
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