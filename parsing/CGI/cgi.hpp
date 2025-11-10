#pragma once
#include "../headers/webserver.hpp"
#include "../../req-res/response.hpp"
#include "../../server/HttpRequest.hpp"
// #include "../../server/Client.hpp"

#define BUFFER 4096


struct HttpRequest;
class Client;
// struct HttpRequest {
//     std::string method;
//     std::string uri;
//     std::string version;
//     std::map<std::string, std::string> headers;
//     std::vector<char> body;
//     std::string boundary;
//     size_t contentLength;
//     size_t headerEndPos;
//     bool   headerParsed;
//     bool   isChunked;
//     size_t maxBodySize;
// };

class cgiHandling
{
    char **env;
    Client *cl;
    int sv[2], childPID;
    DirectoryListing locationConfig;

    void childStart(BaseNode *root,  HttpRequest req);
    public :
        cgiHandling ();
        cgiHandling (Client *cl);
        ~cgiHandling ();

        void setClient(Client *cl);
        int handelCGI(BaseNode *root, HttpRequest req);
        int generateResponse(int fd, std::vector <char> &respoBuff);
        // int getSvForMainP();
};