#include "cgi.hpp"



///////////<<<<>>>>>>////

#include <errno.h>

///////////<<<<>>>>>>////




void printCharP_Vec(std::vector <char *> &vec)
{
    std::vector<char *>:: iterator it = vec.begin();
    std::cout << CYAN << "Printing Vector of Char Pointers \n";
    while(it != vec.end())
    {
        std::cout <<">>" << *it << "<<<\n";
        it++;
    }
    std::cout << DEF<< std::endl; 
}

cgiHandling::cgiHandling ():scriptFullPath("nada"){}
cgiHandling::cgiHandling (Client * val):cl(val), scriptFullPath("nada"){}

cgiHandling::~cgiHandling (){}


void cgiHandling::childStart(BaseNode *root,  HttpRequest &req)
{
    std::string  scriptOutPut, postStrBody;
    std::vector <char> postBody;
    char buff[1024];
    int rd, execRet;

    
    // int index = 0;
    close(sv[1]);
    std::cout << PINK <<"ALLOOO FROM THE CHILD\n"<<DEF;
    std::cout << PINK <<"FDS sv[0]:" << sv[0] << ", sv[1]:"<< sv[1]<<"\n"<<DEF;
    std::cout << PINK <<"SCRIPTFULL PATH" << scriptFullPath<<"\n"<<DEF;
    rd = 1;
    // if (!req.method.compare("POST"))
    // {
    //     while (rd)
    //     {
    //         std::cout << "trying to read \n";
    //         rd = read(sv[0], buff, 1024 - 1);
    //         if (rd < 0)
    //         {
    //             int error = errno;
    //             std::cerr << "Read return Negative\n" << strerror(error)<< std::endl ;

    //             break ;
    //         }
    //         buff[rd] = '\0';
    //         // std::cout << buff << "<<\n";
    //         postStrBody.append(buff);
    //     }
    // }
    // if (postStrBody.length())
    // {
    //     std::cerr << PINK 
    //             << "\n/*******strBody of A Post******/\n"
    //             <<  postStrBody 
    //             << "\n/****************/\n"
    //             << DEF;
    // }
    (void)rd;
    (void)buff;
    // getFullScriptPath(root, req);
      std::cout<< GREEN
             <<"===============\n"
            << scriptFullPath << "\n"
            <<"==============\n"<<DEF;
    getEnvVars(req);
    close(1);
    dup(sv[0]);
    close(0);
    dup(sv[0]);
    std::vector<char*> tmpArgs;
    tmpArgs.push_back(const_cast<char*>(scriptFullPath.c_str()));  // argv[0] = script name
    tmpArgs.push_back(NULL); 
    Env.push_back(NULL);

    execRet = execve(scriptFullPath.c_str(), &tmpArgs[0], &Env[0]);
    close(sv[0]);
    std::cerr << PINK <<"CHILD exits by Failure ! code :" << execRet << "\n"<<DEF;
    exit(1);
    (void)root;
}

void cgiHandling::setClient(Client *val){cl = val;}

int cgiHandling::checkScriptPath(BaseNode *root,HttpRequest &req){
    int retStat;
    struct stat sb;
    getFullScriptPath(root, req);
    

    retStat = stat(scriptFullPath.c_str(), &sb);
    if (retStat!=0)
        return 1;
    if (!S_ISREG(sb.st_mode))
        return 1;
    if (!(S_IXUSR & sb.st_mode))
        return 1;
    return 0;
}
#include <fcntl.h>

int cgiHandling::handelCGI(BaseNode *root, HttpRequest &req)
{
    if (checkScriptPath(root, req))
        return 0;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
        throw (CustomizeError("SocketPair Failed!\n"));
    
    // fcntl(sv[0],F_SETFL, SOCK_NONBLOCK);
    // fcntl(sv[1],F_SETFL ,SOCK_NONBLOCK);


    int flags = fcntl(sv[0], F_GETFL, 0);
    fcntl(sv[0], F_SETFL, flags | O_NONBLOCK);

    flags = fcntl(sv[1], F_GETFL, 0);
    fcntl(sv[1], F_SETFL, flags | O_NONBLOCK);
    
    childPID = fork();
    if (childPID== -1)
    {
        close (sv[0]);
        close (sv[1]);
        throw (CustomizeError("Fork Failed!\n"));
    }
    if (childPID == 0)
        childStart(root, req);
    else
    {
        std::string msgBody(req.body.begin(), req.body.end());
        if (req.method.compare("POST") == 0)
        {
            write(sv[1], msgBody.c_str() , msgBody.size());            
            // shutdown(sv[1], SHUT_WR);
        }
    }
    close(sv[0]);
    return sv[1];
}

void cgiHandling::buildProperReponse(std::vector <char>& respoBuff)
{
    std::string   nessecaryHeader("Content-Type:"),successLine("HTTP/1.1 200 OK\r\n"),respoHeaders, strRespo(respoBuff.begin(), respoBuff.end());
    std::size_t separatorPos, contLength;
    std::ostringstream responseStream;
    
    separatorPos =  strRespo.find("\n\n");
    // if ()

    if (separatorPos == std::string::npos || ( strRespo.compare(0 , 13, nessecaryHeader) && separatorPos > 13 && separatorPos < strRespo.length() - 2))
    {
        respoBuff = buildErrorResponse(500);
        return;
    }
    else
    {
        contLength = respoBuff.size();
        // strRespo = successLine+ "Server: WebServer\r\n" + strRespo;
        // if (contLength < GIGABYTE)
        // {   
        contLength = contLength - (separatorPos + 2);
        responseStream << successLine
                        << "Content-Length: " << contLength << "\r\n";
        respoHeaders = responseStream.str();   
        strRespo = respoHeaders + strRespo;

        // }
        // else
        //     sendChuckedResponse_CGI();
        std::vector <char> tmp(strRespo.begin(), strRespo.end());
        respoBuff = tmp;
    }
}

int cgiHandling::generateResponse(int sv, std::vector <char> &respoBuff)
{
    char buff[BUFFER_SIZE];
    int rd;
    // respoBuff.clear();
    while (1)
    {
        std::cout << RED << "Before ...\n ";
        rd = recv(sv, buff, BUFFER_SIZE - 1, 0);
        std::cout << "return :" << rd
                << "\nAfter ...\n ";
        if (rd >=0)
        {
            buff[rd]='\0';
            respoBuff.insert(respoBuff.end(), buff, buff + rd);
            if (rd == 0)
            {
                std::cout << CYAN << "EOF Reached :\n"<<DEF ;
                break;
            }
        }
        else
        {
            std::cout << ">>Errno: " << errno << " (" << strerror(errno) << ")\n";
            if (errno== EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "No data for the Moment to read\n";
            }
            return 1;
        }
    }

    // std::cout << "Response Readed from the child CGI :\n";
    //         printVecChar(respoBuff);            
    std::cout << "Now Generating a proper one :\n";            
    buildProperReponse(respoBuff);
    // printVecChar(respoBuff);
    return 0;
}


void cgiHandling::getFullScriptPath(BaseNode *root, HttpRequest &req)
{
    DirectoryListing obj;
    std::map <std::string , std::string> :: iterator it = req.headers.find("Host");; 
    std::string hostFound = it->second;
    fillReqStruct(root, obj, req.uri, req.headers.at("Host") );
    scriptFullPath = obj.getRoot() + req.uri;
    // std::cout <<"===============\n"
    //         << scriptFullPath << "\n"
    //         <<"==============\n";
    // p = obj.getRoot() + req.uri;
    // (void)p;
}

std::string genNewForm(std::string &old)
{
    std::string res;
    for (std::size_t i = 0 ; i < old.length(); i++)
    {
        if (old[i] == '-')
            res.push_back('_');
        else
            res.push_back( toupper(old[i]));       
    }
    return res;
}

void cgiHandling::getEnvVars(HttpRequest &req)
{
    std::vector <std::string> :: iterator strIt;
    std::string newForm, oldForm, exceptionCases [3] = {"Authorization", "Content-Type", "Content-Length"};
    std::map<std::string , std::string>:: iterator it = req.headers.begin();
    int i;
    while (it != req.headers.end())
    {
        i = 0;
        while (i < 3)
        {
            if (it->first.compare(exceptionCases[i]) == 0)
            {
                oldForm = it->first;
                newForm = genNewForm(oldForm)+ "=" + it->second;  
            }
            i++;
        }
        if (i == 3 )
        {
            oldForm = "HTTP_"+it->first;
            newForm =  genNewForm (oldForm) + "=" +it->second;
        }
        strEnv.push_back(newForm);
        it++;
    }
    strIt = strEnv.begin();
    Env.clear();
    while(strIt != strEnv.end())
    {
        Env.push_back(const_cast<char *>(strIt->c_str()));
        strIt++;
    }
    // printCharP_Vec(Env);
}

