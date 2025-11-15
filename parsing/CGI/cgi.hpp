#pragma once
#include "../headers/webserver.hpp"
#include "../../req-res/response.hpp"
#include "../../server/HttpRequest.hpp"
// #include "../../server/Client.hpp"

#define BUFFER_SIZE 4096


// struct HttpRequest;
class Client;

class cgiHandling
{
    DirectoryListing locationConfig;
    Client *cl;
    std::string scriptFullPath ;
    std::vector  <char*> Env, Args;
    std::vector  <std::string> strEnv, strArgs;
    int sv[2], childPID;


    void childStart(BaseNode *root,  HttpRequest &req);
    void getFullScriptPath(BaseNode *root, HttpRequest &req);
    // void getFullScriptPath(BaseNode *root, HttpRequest &req, std::string &);
    void getEnvVars( HttpRequest &req);
    void getArgs();
    void buildProperReponse(std::vector<char>&);
    void sendChuckedResponse_CGI(){};
    int checkScriptPath(BaseNode *,HttpRequest &);
    public :
        cgiHandling ();
        cgiHandling (Client *cl);
        ~cgiHandling ();

        void setClient(Client *cl);
        int handelCGI(BaseNode *root, HttpRequest &req);
        int generateResponse(int fd, std::vector <char> &respoBuff);
        
        // int getSvForMainP();
};