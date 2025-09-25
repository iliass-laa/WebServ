#include "../headers/webserver.hpp"



std::string getServerName(ContextNode *server)
{
    DirectiveNode * tmp;
    if (server->type_Context != ServerContext)  
        return std::string ("NOT A SERVER");
    for(int i = 0;i < server->nbrChildsC + server->nbrChildsD; i++)
    {
        if (server->Childs[i]->typeNode == isDirective)
        {
            tmp = dynamic_cast<DirectiveNode *>(server->Childs[i]);
            if (tmp->key.compare("server_name") == 0)
                return tmp->value.back();
        }
    }
    return std::string ("NOT Founded");
}

int getServerPort(ContextNode *server)
{
    int port;
    DirectiveNode * tmp;
    if (server->type_Context != ServerContext)  
        return -1;
    for(int i = 0;i < server->nbrChildsC + server->nbrChildsD; i++)
    {
        if (server->Childs[i]->typeNode == isDirective)
        {
            tmp = dynamic_cast<DirectiveNode *>(server->Childs[i]);
            if (tmp->key.compare("listen") == 0)
            {
                port = std::atoi((tmp->value.back()).c_str());
                return (port);
            }
        }
    }  
    return -1;
}


std::string findRootDir(BaseNode *root)
{
    (void)root;
    return "nad";
}







std::string getRootPath(ContextNode *server)
{
    DirectiveNode * tmp;
    if (server->type_Context != ServerContext)  
        return std::string ("NOT A SERVER");
    for(int i = 0;i < server->nbrChildsC + server->nbrChildsD; i++)
    {
        if (server->Childs[i]->typeNode == isDirective)
        {
            tmp = dynamic_cast<DirectiveNode *>(server->Childs[i]);
            if (tmp->key.compare("root") == 0)
                return tmp->value.back();
        }
    }
    return std::string ("NOT Founded");
}

std::string getRootPathFromLocation(ContextNode *location)
{
     DirectiveNode * tmp;
    if (location->type_Context != LocationContext)  
        return std::string ("NOT A SERVER");
    for(int i = 0;i < location->nbrChildsC + location->nbrChildsD; i++)
    {
        if (location->Childs[i]->typeNode == isDirective)
        {
            tmp = dynamic_cast<DirectiveNode *>(location->Childs[i]);
            if (tmp->key.compare("root") == 0)
                return tmp->value.back();
        }
    }
    return std::string ("NOT FOUND!");

}
std::string getRootPathFromHTTP(ContextNode *http)
{
     DirectiveNode * tmp;
    if (http->type_Context != HttpContext)  
        return std::string ("NOT A SERVER");
    for(int i = 0;i < http->nbrChildsC + http->nbrChildsD; i++)
    {
        if (http->Childs[i]->typeNode == isDirective)
        {
            tmp = dynamic_cast<DirectiveNode *>(http->Childs[i]);
            if (tmp->key.compare("root") == 0)
                return tmp->value.back();
        }
    }
    return std::string ("NOT FOUND");
}


std::string getRootPath(BaseNode *root, t_request req, ContextNode **location)
{
    ContextNode *tmp;
    std::string rootPathHttp,rootPathServer, rootPathLocation, res;
    res = "NOT FOUND!";
    rootPathServer = "NOT FOUND!";
    rootPathLocation = "NOT FOUND!";
    rootPathHttp = "NOT FOUND!";
    tmp = findHttpContext(root);
    if (!tmp)
        return ("No HTTP context founded\n");
    rootPathHttp = getRootPathFromHTTP(tmp);
    tmp = findServerContext(tmp, req.headers.hostname, req.headers.port);
    if (!tmp)
        return ("No Server Context Found\n");
    rootPathServer = getRootPath(tmp);
    //std::cout << "ROOT PATH SERVER :" << rootPathServer << "<<\n";
    tmp = findLocationContext(tmp, req.headers.path);
    //std::cout << "Found this Location :" << tmp->val.back() << "\n";
    if (!tmp)
        return ("NO Location CONTEXT Found\n");
    rootPathLocation = getRootPathFromLocation(tmp);
    res = rootPathLocation;
    if (res.compare("NOT FOUND!") == 0)
        res = rootPathServer;
    if (res.compare("NOT FOUND!") == 0)
        res = rootPathHttp;
    *location = tmp;
    return res;
}
