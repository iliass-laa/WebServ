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


std::vector <std::string> getInterFace(ContextNode *server, int port)
{
    std::string inter;
    std::vector <std::string> res;
    int p;
    DirectiveNode *d;
    for(int i = 0; i< server->nbrChilds; i ++)
    {
        if (server->Childs[i]->typeNode == isDirective)
        {
            d = dynamic_cast<DirectiveNode *>(server->Childs[i]);
            if (d->key.compare("listen") == 0)
            {
                p = getPortFromDirective(d->value.back());
                if (p == port)
                {
                    inter =  getInterFromDirective(d->value.back());
                    res.push_back(inter);
                }
            }
        }
    }

    return res;
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



bool checkBestMatch(std::string uri, std::string locPath, int &nbr)
{
    if (uri.find(locPath) == 0 && static_cast<int>(locPath.length()) > nbr)
    {
        nbr = locPath.length();
        return true;
    }
    return false;
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
    // std::cout <<GREEN<< "serveName :" +serverName + "\nPort :"<<port<< DEF<<std::endl;
    ContextNode *tmp, *res;
    std::vector <int> ports;
    res = NULL;
    if (!http)
        return NULL;
    for (int i = 0 ; i < http->nbrChilds ; i++)
    {
        if (http->Childs[i]->typeNode == isContext)
        {   
            tmp = dynamic_cast<ContextNode *>(http->Childs[i]);
            if (tmp->type_Context == ServerContext)
            {
                if (!getServerPorts(tmp, ports)) 
                {
                    // std::cout <<"Here >>>>>> \n"<<"Ports>>"<< ports.size() << "\n" ;
                    if (!findPort(port, ports))
                    {
                        // std::cout <<"port Front ::"<<ports.front()<<"\nPort back ::"<< ports.back() << "\n" ;

                        if (!res)
                            res = tmp;
                        if (!serverName.compare(getServerName(tmp)) || serverNameisInterface(serverName,getInterFace(tmp, port)))
                            return tmp;
                        // std::cout <<"Server Name in Conf>>>>>>>" +getServerName(res)+ "<<<<<\n";
                        // std::cout << " Server Name Arg>>>>>>>" +serverName+ "<<<<<\n";
                    }
                }
            }
        }
    }
    // if (res)
    //     std::cout << "LLAAASST\n";
    return res;
}

ContextNode *findLocationContext(ContextNode* server, std::string path)
{
    ContextNode *tmp, *tmpRes;
    std:: size_t pos;
    int def , bestMatch;

    def= 0;
    bestMatch = 0;
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
    // int max = server->nbrChildsC + server->nbrChildsD;
    // std::cout << GREEN <<"\n>>>>>MAX :"<< max <<"<<\n" <<DEF; 
    // std::cout << GREEN <<">>>>>MAX :"<< server->nbrChildsC + server->nbrChildsD <<"<<\n" <<DEF; 
    // for(int i = 0;  i < max ; i++)
    for(int i = 0;  i < server->nbrChilds ; i++)
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
                else if(checkBestMatch(path, tmp->val.back(), bestMatch))
                    tmpRes = tmp;
            }
            else if(tmp->val.back().compare(path) == 0)
                return tmp;
        }
    }
    return tmpRes;
}