#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP


#include "includes.hpp"
#include "structs.hpp"
#include "config.hpp"
#include "classes.hpp"
#include "AST.hpp"
#include "colors.hpp"
#include "../../server/Core.hpp"



void testSocket(char *path, BaseNode *root);
// void parseConfigFile(char *path, ConfigFileInfos &obj);
// void parseConfigFile(char *path);
BaseNode *parseConfigFile(const char *path);
int parseRequest(std::string request, t_request &req);
int sendtheRightResponse(int client_fd,  t_request &req, BaseNode *root);
// int sendtheRightResponse(int client_fd,t_request req);

BaseNode *buildTree(TokenizerData &tk);
void printTree(BaseNode * root, int);
void freeTree(BaseNode *root);
void free_D_Array(char **env);


DirectiveNode *findDirectiveChild(ContextNode *parent, std::string childName);
ContextNode *findContextChild(ContextNode *parent, std::string childName);



/******************CORE:***************** */


void printRequest(t_request req);
void core(std::string reqStr, int client_fd, BaseNode *root);

/******************Great Tools:***************** */
std::string getServerName(ContextNode *server);
// std::string getServerName(ContextNode *server, std::vector <int> &ports);
// std::string getServerName(ContextNode *server, std::vector <int> &ports);
int getServerPort(ContextNode *server);
int getServerPorts(ContextNode *server, std::vector<int> &ports);

int findPort(int port, std::vector<int> ports);


// ContextNode *findServerContext(ContextNode* http, std::string serverName);
// ContextNode *findStaticLocation(BaseNode* root, std::string serverName, std::string locationPath);
ContextNode *findStaticLocation(BaseNode* root, t_request &req, std::string locationPath);

std::string getRootPath(ContextNode *server);
std::string getRootPath(BaseNode *root, t_request req, ContextNode **location);

/******************Find Contexts:***************** */

ContextNode *findHttpContext(BaseNode* root);
ContextNode *findServerContext(ContextNode* http, std::string serverName , int port);
ContextNode *findLocationContext(ContextNode* server, std::string path);


/******************Response Tools:***************** */

std::string getFullPath(t_request &req, std::string rootPath, int *f_or_D, std::string &fileName );
int foundIndex(ContextNode *location, std::vector<std::string> &indexes);
int dirListing(ContextNode *location);
void    redirectClient(int client_fd, std::string dir);



/******************Response Sending..:***************** */

int serveFile(int client_fd, DIR *dir,std::string fileTarget, std::string fullPath);
void serveDir(int client_fd, DIR *dir, std::string dirName);
void    sendSuccesHeader(int client_fd);
void    errorHandling(int client_fd,enum errors errn);



/******************  CGI..:***************** */
int    handleCGI(int client_fd,t_request &req,BaseNode *root);
char **getEnv(t_request &req);


/******************  CGI>>TOOLS..:***************** */

ContextNode* findRightServer(t_request &req,BaseNode *root);
ContextNode     *findRightLocation(t_request &req, ContextNode *server);
void getReqInfos(t_request & req,BaseNode *root);
char *ft_strcopy(const char *s, int size);
void handleCGIError(t_request &req, int client_fd);

void fillServerConf(BaseNode *root, Core &obj);
void printPairs(std::set<std::string > &pairs);

#endif
