#include "../../headers/webserver.hpp"
#include "../../../server/Core.hpp"
#include <set>



void addPair(std::string val,  std::set <std::string> &pairs)
{
    std::string defaultInter("0.0.0.0"), newVal;

    newVal = val;
    if (val.find(":") == std::string::npos)
        newVal = (defaultInter + ":"+ val);
    pairs.insert(val);
}

void fillServerConf(BaseNode *root, Core &obj)
{

    std::set <std::string> pairs;
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
                        addPair(dNode->value.back(), pairs);
                    }
                }
            }   
        }
    }
    obj.setPairs(pairs);
}