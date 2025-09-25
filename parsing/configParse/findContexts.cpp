#include "../headers/webserver.hpp"

int findInV(std::vector<std::string>v, std::string s)
{
    for(size_t i = 0; i <  v.size(); i++)
    {
        if (v[i].compare(s) == 0)
            return 0;
    }
    return 1;
}

ContextNode *findStaticLocation(BaseNode* root, t_request &req, std::string locationPath)
{
    ContextNode * tmp, *tmp2;

    tmp = findHttpContext(root);
    if (!tmp)
        return NULL;
    tmp = findServerContext(tmp, req.headers.hostname, req.headers.port);
    if (!tmp)
        return NULL;
    for (int i =0;i < tmp->nbrChildsC + tmp->nbrChildsD ; i++)
    {
        if (tmp->Childs[i]->typeNode == isContext)
        {
            tmp2 = dynamic_cast<ContextNode *>(tmp->Childs[i]); 
            if (!findInV(tmp2->val, locationPath))
                return tmp2;
        }
    }
    return NULL;
}



ContextNode *findHttpContext(BaseNode* root)
{
    ContextNode *tmp, *res;

    tmp = dynamic_cast<ContextNode *>(root);

    if (tmp->type_Context == HttpContext)
        return tmp;
    if (tmp->type_Context == MainContext)
    {
        for (int i= 0; i < tmp->nbrChildsC + tmp->nbrChildsD ; i++)
        {
            if (tmp->Childs.at(i)->typeNode== isContext)
            {
                res = dynamic_cast<ContextNode *>(tmp->Childs.at(i));
                if (res->type_Context == HttpContext)
                    return res;
            }
        }
    } 
    return NULL;
}


ContextNode *findServerContext(ContextNode* http, std::string serverName , int port)
{
    ContextNode *tmp, *res;

    res = NULL;
    for (int i = 0 ; i < http->nbrChildsC + http->nbrChildsD ; i++)
    {
        if (http->Childs[i]->typeNode == isContext)
        {   
            tmp = dynamic_cast<ContextNode *>(http->Childs[i]);
            if (tmp->type_Context == ServerContext && port == getServerPort(tmp))
            {
                if (!res)
                    res = tmp;
                if (serverName.compare(getServerName(tmp)) == 0)
                    return tmp;
            }
        }
    }
    return res;
}

ContextNode *findLocationContext(ContextNode* server, std::string path)
{
    ContextNode *tmp, *tmpRes;
    std:: size_t pos;
    
    int def = 0;
    tmpRes = NULL;
    if (!server)
    {
        std::cout << RED << "Server is NULL\n"<< DEF;
        return NULL; 
    }
    // std::cout << RED << "Context Type:"<<server->type_Context;
    // <<" val :"+server->val.back()+"\n"<< DEF;
    if ( server->type_Context != ServerContext)
    {
        std::cout << RED << "Not a Server:"<<server->type_Context<<"\n"<< DEF;
        return NULL;
    }
    if (path.compare("/") == 0)
        def = 1; 
    int max = server->nbrChildsC + server->nbrChildsD;
    // std::cout << GREEN <<"\n>>>>>MAX :"<< max <<"<<\n" <<DEF; 
    // std::cout << GREEN <<">>>>>MAX :"<< server->nbrChildsC + server->nbrChildsD <<"<<\n" <<DEF; 
    for(int i = 0;  i < max ; i++)
    {
        if (server->Childs[i]->typeNode == isContext)
        {
            tmp = dynamic_cast<ContextNode *>(server->Childs[i]);
            if (!tmpRes && tmp->val.back().compare("/") == 0)
                tmpRes = tmp;
            if (!def)
            {
                pos = tmp->val.back().find(path); 
                if ( pos != std::string::npos && pos == 0)
                    return tmp;
            }
            else if(tmp->val.back().compare(path) == 0)
                return tmp;
        }
    }
    return tmpRes;
}