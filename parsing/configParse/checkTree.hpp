#include "../headers/webserver.hpp"


class checkTree 
{
    std::map <std::string, std::map <std::string , std::vector <std::string > > > ContextSource;
    
    std::set<int > allowedContexts;
    std::set <std::string > allowedDirectives;
    std::map <std::string , std::set<int> > allowedDirectivesPerContext;
    std::map <int, int> allowedContextPerLevel;
    int     checkDirectiveContent(DirectiveNode *, std::string &);
    int     checkContext(BaseNode *, int);
    int     checkDirective(DirectiveNode* , ContextNode *);
    std::pair <std::string , std::set<int> > getPair(std::string &);
    public :
        checkTree();
        ~checkTree();

        int finalChecking(BaseNode *root);
};