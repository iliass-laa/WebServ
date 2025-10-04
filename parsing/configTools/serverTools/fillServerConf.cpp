#include "../../headers/webserver.hpp"
#include "../../../server/Core.hpp"
#include <set>



void addPair(std::string val,  std::set <std::string> &pairs)
{
    std::string defaultInter("0.0.0.0"),port, inter, newVal;
    std::set <std::string> :: iterator it;
    int pos;

    newVal = val;
    it = pairs.begin();
    pos = val.find(":");
    if (pos == std::string::npos)
        newVal = (defaultInter + ":"+ val);
    
    port = newVal.substr(pos+1, newVal.length());
    inter =  newVal.substr(0 , pos);
    if (inter.compare("0.0.0.0") == 0)
    {
        for(it = pairs.begin(); it != pairs.end(); it++)
        {
            if (it->find(port) != std::string::npos)
                pairs.erase(it);
        }
    }
    pairs.insert(newVal);
}

void fillServerConf(BaseNode *root, Core &obj)
{

    std::set <std::string> pairs, serverPairs;
    ContextNode *serverNode, *httpNode;
    DirectiveNode *dNode;

    httpNode = findContextChild( dynamic_cast<ContextNode *>(root),"http"); 
    for (int i = 0 ; i < httpNode->nbrChilds; i++)
    {
        if (httpNode->Childs[i]->typeNode == isContext)
        {
            serverNode =  dynamic_cast<ContextNode *>(httpNode->Childs[i]);

            for(int j = 0; j < serverNode->nbrChilds; j++)
            {
                if (  httpNode->Childs[i]->typeNode == isDirective)
                {
                    dNode =  dynamic_cast<DirectiveNode *>(serverNode->Childs[i]);
                    if (dNode->key.compare("listen") == 0)
                    {
                        addPair(dNode->value.back(), serverPairs);
                    }
                }
            }
            pairs.insert(serverPairs.begin(), serverPairs.end());   
        }
    }
    obj.setPairs(pairs);
}