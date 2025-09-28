#ifndef AST_HPP
#define AST_HPP 

#include "includes.hpp"

#define  MainContext 0
#define  EventsContext 1
#define  HttpContext 2
#define  TypesContext 3
#define  ServerContext 4
#define  LocationContext 5


#include <list>


enum NodeType{
    isDirective,
    isContext
};


class BaseNode 
{  
    public :
        BaseNode(){};
        BaseNode(NodeType typeNode):typeNode(typeNode){};
        virtual ~BaseNode(){};
        NodeType typeNode;
};

class DirectiveNode : public BaseNode
{
    public :

        DirectiveNode():BaseNode(isDirective){};
        ~DirectiveNode(){};
        std::string key;
        std::vector <std::string> value;
};

class ContextNode :  public BaseNode
{
    public :
        ContextNode():BaseNode(isContext){
            nbrChildsC = 0;
            nbrChildsD = 0;
        };
        ~ContextNode(){};
        int type_Context;
        int nbrChildsC;
        int nbrChildsD;
        int nbrChilds;
        std::vector <std::string> val;
        std::vector <BaseNode *> Childs;
};




#endif