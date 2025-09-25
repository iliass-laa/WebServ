#include "../../headers/webserver.hpp"


void fillServerConf(BaseNode *root, )
{
    ContextNode *httpNode = findContextChild( dynamic_cast<ContextNode *>(root),"http"); 

    for(int i = 0 ; i < httpNode->nbrChilds ; i++ )
    {
        if (httpNode->Childs[i]->typeNode == isContext)
        {
            
        }
    }
}