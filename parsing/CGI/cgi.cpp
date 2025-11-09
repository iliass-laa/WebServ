#include "cgi.hpp"


cgiHandling::cgiHandling (){}
cgiHandling::cgiHandling (Client * val):cl(val){}

cgiHandling::~cgiHandling (){}


void cgiHandling::childStart(BaseNode *root,  HttpRequest req)
{
    std::string scriptFullPath;
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
    std::cout << PINK << "CGI DONE EXEC THE SCRIPT \n" << DEF ;
    write(sv[0], "Done\n", 5);
    close(sv[0]);
    std::cout << PINK <<"CHILD exits !\n"<<DEF;
    exit(0);
    // execve()
    (void)root;
    (void)req;
}

void cgiHandling::setClient(Client *val){cl = val;}

int cgiHandling::handelCGI(BaseNode *root, HttpRequest req)
{
    // setInfo(req);
    int pid;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
        throw (CustomizeError("SocketPair Failed!\n"));
    pid = fork();
    if (pid== -1)
        throw (CustomizeError("Fork Failed!\n"));
    if (pid == 0)
        childStart(root, req);
    else    
        childPID = pid;
    close(sv[0]);
    return sv[1];
}

int cgiHandling::generateResponse()
{
    return 0;
}