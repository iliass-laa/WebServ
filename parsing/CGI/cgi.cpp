#include "cgi.hpp"


cgiHandling::cgiHandling (){}
cgiHandling::cgiHandling (Client * val):cl(val){}

cgiHandling::~cgiHandling (){}


void cgiHandling::childStart(BaseNode *root,  HttpRequest req)
{
    std::string scriptFullPath, scriptOutPut;
    char buff[1024];
    int rd;
    close(sv[1]);
    std::cout << PINK <<"ALLOOO FROM THE CHILD\n"<<DEF;
    std::cout << PINK <<"FDS sv[0]:" << sv[0] << ", sv[1]:"<< sv[1]<<"\n"<<DEF;
    if (!req.method.compare("POST"))
        std::cout << "FROM CGI it's a POST :: \n";
    if (!req.method.compare("GET"))
        std::cout << "FROM CGI it's a GET :: \n";
    rd = 1;
    if (!req.method.compare("POST"))
    {
    while (rd)
        {
            std::cout << "trying to read \n";
            rd = read(sv[0], buff, 1024 - 1);
            buff[rd] = '\0';
            std::cout << buff << "<<\n";
        }
    }

    scriptOutPut = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/plain\r\n"
                       "Content-Length: 11\r\n"
                    //    "Connection: close\r\n"
                       "\r\n"
                       "Hello World";
    std::cout << PINK << "CGI DONE EXEC THE SCRIPT \n" << DEF ;
    // getScriptFullPath(root, req);
    send(sv[0], scriptOutPut.c_str(), scriptOutPut.length(), 0);
    close(1);
    dup(sv[0]);
    execve(scriptFullPath.c_str(), NULL, env);

    close(sv[0]);
    std::cout << PINK <<"CHILD exits !\n"<<DEF;
    exit(010);
    (void)root;
    (void)req;
}

void cgiHandling::setClient(Client *val){cl = val;}

int cgiHandling::handelCGI(BaseNode *root, HttpRequest req)
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
        throw (CustomizeError("SocketPair Failed!\n"));
    childPID = fork();
    if (childPID== -1)
    {
        close (sv[0]);
        close (sv[1]);
        throw (CustomizeError("Fork Failed!\n"));
    }
    if (childPID == 0)
        childStart(root, req);
    close(sv[0]);
    return sv[1];
}

int cgiHandling::generateResponse(int sv, std::vector <char> &respoBuff)
{
    char buff[BUFFER];
    int rd;
    respoBuff.clear();
    while (1)
    {
        rd = recv(sv, buff, BUFFER - 1, 0);
        buff[rd]='\0';
        respoBuff.insert(respoBuff.end(), buff, buff + rd);
        if (rd == 0)
            break;
    }
    return 0;
}