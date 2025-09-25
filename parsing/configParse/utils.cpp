#include "../headers/webserver.hpp"



void printSpaces(int level)
{
    for(int i = 0; i < level; i++)
    {
        std::cout << " ";
    }
    std::cout << "->";
}

void    printDirective( DirectiveNode * root,int Level)
{
    printSpaces(Level);
    std::cout <<"D_Key :" + root->key +  ":: ";
    std::cout <<"D_value :";
    for(int i = 0 ; static_cast<size_t>(i) < root->value.size(); i++)
    {
        // printSpaces(Level);
        std::cout << root->value[i]<<  " ";
    }
    std::cout<< "\n";
}


void    printTypContextandVal(int type, std::vector <std::string> &val, int level, int nbrChilds)
{
    std::string Contexts[6] = {"main", "events","http", "types", "server", "location"};
    for(int i = 0; i < 6;i++)
    {
        if (i==type)
        {
            std::cout << "type Of Context:"+ Contexts[i] +  "( Level:" <<(level / 3) << ") ";
            std::cout << ",Nbr of Childerens : " << nbrChilds << "," ;
        }
    }
    for(size_t i = 0; i < val.size();i++)
        std::cout << val[i] << " ";
    std::cout << "\n";
}


void    printfContext(ContextNode *node, int level)
{
    printSpaces(level);
    printTypContextandVal(node->type_Context, node->val,level, node->nbrChildsD + node->nbrChildsC);
    for(size_t i = 0; i < node->Childs.size();i++)
    {
        if (node->Childs[i]->typeNode == isDirective)
            printDirective(dynamic_cast<DirectiveNode*>(node->Childs[i]), level);
        else
            printfContext(dynamic_cast<ContextNode*>(node->Childs[i]), 3 + level);
    }
}


void printTree(BaseNode * root, int Level)
{
    printfContext(dynamic_cast<ContextNode*>(root), Level);
}




ContextNode *findContextChild(ContextNode *parent, std::string childName)
{
    ContextNode *child;
    if (parent == NULL)
        return NULL;
    for(int i=0 ; i< parent->nbrChildsC+parent->nbrChildsD;i++)
    {
        if (parent->Childs[i]->typeNode == isContext)
        {
            child = dynamic_cast<ContextNode *>(parent->Childs[i]);
            if (child->val.at(0).compare(childName) == 0)
                return child;
        }
    }
    return NULL;
}


DirectiveNode *findDirectiveChild(ContextNode *parent, std::string childName)
{
    DirectiveNode *child;
    if (parent == NULL)
        return NULL;
    for(int i=0 ; i< parent->nbrChildsC+parent->nbrChildsD;i++)
    {
        if (parent->Childs[i]->typeNode == isDirective)
        {
            child = dynamic_cast<DirectiveNode *>(parent->Childs[i]);
            if (child->key.compare(childName) == 0)
                return child;
        }
    }
    return NULL;
}
