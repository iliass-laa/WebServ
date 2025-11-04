#pragma once
#include "../headers/webserver.hpp"


class cgiInfo{
    int cgiFd;
    Client *cl;
    
    public :
        cgiInfo();
        cgiInfo();

};


class cgiHandling
{
    // char 

    std::vector <int> ScriptId;
    
    // void cgiExec();

    public :
        cgiHandling ();
        ~cgiHandling ();

        void handelCGI(BaseNode *, HttpRequest, std::vector<char> &);
};