#include "../../headers/webserver.hpp"
#include "../../../server/Core.hpp"
#include <set>


void printPairs(std::set<std::string > &pairs)
{
    std::set<std::string > :: iterator it = pairs.begin();
    std::cout << "printing Pairs\n";
    while(it != pairs.end())
    {
        std::cout << *it<< "\n";
        it++;
    }
}

int isGlobal(std::string pair)
{
    if (pair.find("0.0.0.0") == 0)
        return 1;
    return 0;
}

int theSamePorts(std::string pair,std::set<std::string > globals)
{
    std::string portStr("");
    std::set <std::string> :: iterator it;
    std::size_t pos;
    pos  = pair.find(":");
    if (pos != std::string::npos)
        portStr = pair.substr(pos+1, pair.length());
    it = globals.find("0.0.0.0:"+portStr);
    if (it != globals.end())
        return 1;
    return 0;
}
void lastCheck (std::set<std::string > &pairs)
{
    std::set<std::string > :: iterator it, toErase;
    std::set<std::string> globals;
    std::size_t pos;
    it = pairs.begin();
    while(it != pairs.end())
    {
        pos = it->find("0.0.0.0");
        if (pos != std::string::npos && pos == 0)
            globals.insert(*it);
        it++;
    }
    it = pairs.begin();
    while(it != pairs.end())
    {
        if (!isGlobal(*it) && theSamePorts(*it, globals))
        {
            toErase = it++;
            pairs.erase(toErase);
        }
        else
            it++;
    }
    // if (pairs.empty())
        // throw (ConfigFileError("Need Some Pairs\n"));
}

void addPair(std::string val,  std::set <std::string> &pairs)
{
    std::string errMsg("Job for webServ.service failed because the control process exited with error code.");
    std::string defaultInter("0.0.0.0"),port, inter, newVal;
    std::set <std::string> :: iterator it, toErase;
    size_t pos;
    int skip;

    newVal = val;
    skip = 0;
    it = pairs.begin();
    pos = newVal.find(":");
    if (pos == std::string::npos)
        newVal = (defaultInter + ":"+ val);
    pos = newVal.find(":");
    port = newVal.substr(pos+1, newVal.length());
    inter =  newVal.substr(0 , pos);
    if (pairs.find(newVal) != pairs.end())
        throw(ConfigFileError(errMsg + "\nCheck this pair (duplicated on the same server Context):" + newVal));
    if (inter.compare(defaultInter) == 0)
    {
        while (it!= pairs.end())
        {
            if (it->find(port) != std::string::npos)
            {
                toErase = it++;
                pairs.erase(toErase);
            }
            else
                it++;
        }
    }
    else
    {
        it = pairs.find(defaultInter +":"+ port);
        if (it != pairs.end())
            skip = 1;
    }
    if (!skip)
        pairs.insert(newVal);
}

void fillServerConf(BaseNode *root, Core &obj)
{

    std::set <std::string> pairs, serverPairs;
    ContextNode *serverNode, *httpNode;
    DirectiveNode *dNode;

    httpNode = findHttpContext( dynamic_cast<ContextNode *>(root)); 
    for (int i = 0 ; i < httpNode->nbrChilds; i++)
    {
        if (httpNode->Childs[i]->typeNode == isContext)
        {
            serverNode =  dynamic_cast<ContextNode *>(httpNode->Childs[i]);
            if (serverNode->type_Context == ServerContext)
            {
                serverPairs.clear();
                for(int j = 0; j < serverNode->nbrChilds; j++)
                {
                    if ( serverNode->Childs[j]->typeNode == isDirective)
                    {
                        dNode =  dynamic_cast<DirectiveNode *>(serverNode->Childs[j]);
                        if (dNode->key.compare("listen") == 0)
                            addPair(dNode->value.back(), serverPairs);
                    }
                }
                pairs.insert(serverPairs.begin(), serverPairs.end());   
            }
        }
    }
    lastCheck(pairs);
    obj.setPairs(pairs);
}