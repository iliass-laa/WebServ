#include "checkTree.hpp"


std::pair <std::string , std::set<int> > checkTree::getPair(std::string &D)
{
    std::pair <std::string , std::set<int> > res;
    std::set <int> C;
    res.first = D;
    if (!D.compare("root") || !D.compare("index") || !D.compare("absolute_redirect") || !D.compare("autoindex") || !D.compare("client_max_body_size"))
    {
        C.insert(HttpContext);
        C.insert(ServerContext);
        C.insert(LocationContext);
    }
    else if (!D.compare("server_name") || !D.compare("listen"))
        C.insert(ServerContext);
    else if (!D.compare("limit_except"))
    {
        C.insert(ServerContext);
        C.insert(LocationContext);
    }
    else 
        throw (ConfigFileError("Unkown Directive:" + D));
    res.second = C;
    return res;
}

checkTree::~checkTree(){}

checkTree::checkTree()
{
    std::string allD[8] = {"root", "index", "listen", "server_name", "absolute_redirect", "autoindex", "client_max_body_size", "limit_except"};
    for (int i= 0; i < 8; i++)
    {
        if (i < 4)
        {   
            // allowedContextPerLeve.insert();
            allowedContexts.insert(i);
        }
        allowedDirectives.insert(allD[i]);
        allowedDirectivesPerContext.insert(getPair(allD[i]));
    }
}



int checkTree::checkContext(BaseNode*Context, int Level)
{
    std::string tc [4] = {"main", "http", "server", "location"};
    ContextNode *Cn;
    int res , i;
    res= 0;
    i = 0;
    Cn = dynamic_cast<ContextNode*>(Context);  
    if (Level != Cn->type_Context)
        throw (ConfigFileError ("Context is on the Wrong Level !"));
    if (Cn->type_Context == MainContext && (Cn->nbrChilds != 1 || Cn->Childs[0]->typeNode != isContext))
    {

        throw (ConfigFileError ("For the Moment the Main Context can have only one child which is Http Context !"));
    }
    if ((Cn->type_Context == LocationContext && Cn->val.size() != 1) || (Cn->type_Context != LocationContext && Cn->val.size() != 0))
        throw (ConfigFileError ("invalid number of arguments in " + tc[Cn->type_Context]));
    while  (i < Cn->nbrChilds)
    {
        if (i >= Cn->nbrChilds )
            break;
        if (Cn->Childs[i] && Cn->Childs[i]->typeNode == isDirective)
            res = checkDirective(dynamic_cast<DirectiveNode*>(Cn->Childs[i]), Cn);
        else if ((Cn->Childs[i]->typeNode == isContext))
            res = checkContext(Cn->Childs[i], Level + 1);
        if (res)
            return res;
        i++;
    }
    return res;
}

int checkTree::checkDirective(DirectiveNode*Dn , ContextNode *Cn)
{
    std::set<int> tmp;
    std::string dStr;
    std::set <std::string>::iterator it ; 

    it = allowedDirectives.find(Dn->key) ;
    if (it == allowedDirectives.end())
        throw ConfigFileError("Unallowed Directive :" + Dn->key );  
    tmp = allowedDirectivesPerContext.at(Dn->key);
    if (tmp.find(Cn->type_Context) == tmp.end())
        throw ConfigFileError("Directive '" + Dn->key + "' Not Allowed on this Context");
    dStr = *it;
    if (checkDirectiveContent(Dn, dStr))
        throw ConfigFileError("Directive '" + Dn->key + "' Contains unrecognized Content");
    return 0;
}

int   checkTree::checkDirectiveContent(DirectiveNode *Dn, std::string &dStr)
{   
    std::size_t pos;
    std::string valStr;
    std::vector <std::string> val = Dn->value;
    if (!dStr.compare("root") &&(val.size() != 1 || val.back().length() == 0))
        return 1;
    else if (!dStr.compare("index") && (val.size() == 0 ||val.back().length() == 0))
        return 1;
    else if (!dStr.compare("server_name") &&(val.size() != 1 || val.back().length() == 0))
        return 1;
    else if (!dStr.compare("autoindex") )
    {
        if (val.size() != 1 || val.back().length() == 0 || (val.back().compare("on") && val.back().compare("off")))
            return 1;
    }
    else if (!dStr.compare("limit_except"))
    {
        if (val.size() == 0 || (val.back().compare("GET") && val.back().compare("POST") &&  val.back().compare("DELETE")))
            return 1;
    }
    else if (!dStr.compare("listen"))
    {
        valStr = val.back();
        if (val.size() != 1 || (valStr.length() == 0))
            return 1;
        pos = valStr.find(":") ;
        if (pos != std::string::npos)
            valStr = valStr.substr(pos +1 , valStr.length());
        if (valStr.find_first_not_of("0123456789") != std::string::npos)
                return 1;
    }
    return 0;
}


int checkTree::finalChecking(BaseNode *root)
{
    checkContext(root, 0);
    return 0;
}