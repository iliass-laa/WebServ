#include "../headers/webserver.hpp"


int getEndOfContext(TokenizerData &tk, int start, int &nbrC,int &nbrD)
{
    int first, nbrBraquets = 0;
    first = start;
    while (static_cast<size_t>(start) < tk.tokens.size())
    {
        
        if (tk.tokens[start].compare("{") == 0 )
        {
            if (nbrBraquets==1)
                nbrC++;
            nbrBraquets ++;
        }
        else if (tk.tokens[start].compare("}") == 0)
        {
            if (!nbrBraquets)
                throw(ConfigFileError());
            nbrBraquets --;
            if (!nbrBraquets && first)
                return start;
        }
        if (nbrBraquets == 1 && tk.tokens[start] == ";")
            nbrD++;
        start ++;
    }
    return start;
}




int getContextType(TokenizerData &tk,int index)
{
    std::string Contexts[6] = {"main", "events","http", "types", "server", "location"};
    for(int i = 0; i < 6;i++)
    {
        if (tk.tokens[index].compare(Contexts[i]) == 0)
            return i;
    }
    return -1;
}


void getVal(ContextNode *Node, TokenizerData &tk, int &start, int typeC)
{
    if (!typeC)
        return;
    while (tk.tokens[start].compare("{") && static_cast<size_t>(start) < tk.tokens.size())
        Node->val.push_back(tk.tokens[start++]);
}

BaseNode * createDirectiveNode(TokenizerData &tk, int &start){
    
    DirectiveNode *Node;
    Node = new DirectiveNode();
    Node->key =tk.tokens[start];
    
    start++;
    while (static_cast<size_t>(start) < tk.tokens.size())
    {
        if (tk.tokens[start].compare(";") ==0)
            break;
        Node->value.push_back(tk.tokens[start]);
        start++;
    }
    start++;
    return Node;
}


BaseNode *creatContextNode(TokenizerData &tk, int &start, int typeC)
{
    ContextNode *Node;
    int typeOfContext, end;

    Node = new ContextNode();
    Node->type_Context = typeC;
    if (typeC != MainContext)
        getVal(Node, tk, start, typeC);
    end = getEndOfContext(tk, start, Node->nbrChildsC, Node->nbrChildsD);
    // std::cout << "type of COntext: " << typeC << "\n";
    // std::cout << "--->start of COntext: " << start << " ";
    // std::cout << "End of COntext: " << end << "\n";
    
    if (tk.tokens[start].compare("{") == 0)
        start ++;
    while(start < end)
    {
        typeOfContext = getContextType(tk, start);
        if (typeOfContext > 0)
        {
            start++;
            Node->Childs.push_back(creatContextNode(tk, start, typeOfContext));
        }
        else
        {
            Node->Childs.push_back(createDirectiveNode(tk, start));
        }
    }
    if (static_cast<size_t>(start) <  tk.tokens.size() )
    {
        if (tk.tokens[start].compare("}") == 0)
            start++;
    }
    return Node;
}


BaseNode *buildTree(TokenizerData &tk)
{
    BaseNode *Node;
    int start = 0;
    Node = creatContextNode(tk, start, MainContext);
    return Node;
}