#include "../headers/webserver.hpp"



void printSpaces(int level)
{
    for(int i = 0; i < level; i++)
    {
        std::cout << " ";
    }
    std::cout << "=>";
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
    std::string Contexts[4] = {"main", "http", "server", "location"};
    for(int i = 0; i < 4;i++)
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
            printDirective(dynamic_cast<DirectiveNode*>(node->Childs[i]),3 + level);
        else
            printfContext(dynamic_cast<ContextNode*>(node->Childs[i]), 3 + level);
    }
}


void printTree(BaseNode * root, int Level)
{
    std::cout << "Printing Tree\n";
    printfContext(dynamic_cast<ContextNode*>(root), Level);
    std::cout << "Done Printing Tree\n";
}




ContextNode *findContextChild(ContextNode *parent, std::string childName)
{
    ContextNode *child;
    if (parent == NULL)
        return NULL;
    // std::cout << " >>>httpNode->nbrChilds:" << parent->nbrChildsC+parent->nbrChildsD << "\n";
    // std::cout << " >>>httpNode->nbrChilds:" << parent->nbrChilds << "\n";
    // std::cout << " >>>httpNode->Childs1Type:" << parent->Childs[0]->typeNode << "\n";
    for(int i=0 ; i< parent->nbrChildsC+parent->nbrChildsD;i++)
    {
        if (parent->Childs[i]->typeNode == isContext)
        {
            child = dynamic_cast<ContextNode *>(parent->Childs[i]);
            // std::cout << child->val.back() + " ,"+ childName<<  "\n";
            if (child->val.back().compare(childName) == 0)
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
