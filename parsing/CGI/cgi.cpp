#include "cgi.hpp"



///////////<<<<>>>>>>////

#include <errno.h>

///////////<<<<>>>>>>////




void printCharP_Vec(std::vector <char *> &vec)
{
    std::vector<char *>:: iterator it = vec.begin();
    // std::cout << CYAN << "Printing Vector of Char Pointers \n";

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
    std::string  scriptOutPut;
    char buff[1024];
    int rd, execRet;
    close(sv[1]);
    std::cout << PINK <<"ALLOOO FROM THE CHILD\n"<<DEF;
    std::cout << PINK <<"FDS sv[0]:" << sv[0] << ", sv[1]:"<< sv[1]<<"\n"<<DEF;
    std::cout << PINK <<"SCRIPTFULL PATH" << scriptFullPath<<"\n"<<DEF;
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



    getFullScriptPath(root, req, scriptFullPath);
    getEnvVars(req);
    // getArgs();

    // std::cout <<YELLOW
    //         << "CGI :: iam executing this Script:\n"
    //         << "path: " << scriptFullPath << "\n"
    //         << "pathAdr :" << &scriptFullPath<< "\n"
    //         <<   "this obj addr:" << this
    //         << "\n"
    //         <<DEF;
    // printCharP_Vec(Env);
    close(1);
    dup(sv[0]);
    // char **tmpArgs = NULL;
    // execRet = execve(scriptFullPath.c_str(), args, env);
    // execRet = execve(scriptFullPath.c_str(), &Args[0], &Env[0]);
    // scriptFullPath = "/bin/ls";

    std::vector<char*> tmpArgs;
    tmpArgs.push_back(const_cast<char*>(scriptFullPath.c_str()));  // argv[0] = script name
    // Add any other arguments here
    tmpArgs.push_back(NULL);  // NULL-terminate
    Env.push_back(NULL);

// Execute

    execRet = execve(scriptFullPath.c_str(), &tmpArgs[0], &Env[0]);
    // std::cerr << PINK << "CGI DONE EXEC THE SCRIPT \n"  ;
    // std::cerr <<  "errno :" << errno << "\n"
    //         << "executable Path ::"  << scriptFullPath<< "\n";
    // std::cerr <<  "errno :" << strerror(errno) << "\n";

    close(sv[0]);
    std::cerr << PINK <<"CHILD exits by Failure ! code :" << execRet << "\n"<<DEF;
    exit(010);
    (void)root;
    (void)req;
}

void cgiHandling::setClient(Client *val){cl = val;}

int cgiHandling::handelCGI(BaseNode *root, HttpRequest &req)
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
    // else
    // {
    //     std::cout <<YELLOW
    //         << "CGI :: iam executing this Script:\n"
    //         << "path: " << scriptFullPath << "\n"
    //         << "pathAddr :" << &scriptFullPath<< "\n"
    //         <<   "this obj addr:" << this
    //         << "\n"
    //         <<DEF;
    // }
    close(sv[0]);
    return sv[1];
}

void cgiHandling::buildProperReponse(std::vector <char>& respoBuff)
{
    std::string   nessecaryHeader("Content-Type:"),successLine("HTTP/1.1 200 OK\r\n"),respoHeaders, strRespo(respoBuff.begin(), respoBuff.end());
    std::size_t separatorPos, contLength;
    std::ostringstream responseStream;
    
    separatorPos =  strRespo.find("\n\n");
    if (strRespo.compare(0 , 13, nessecaryHeader) && separatorPos > 13 && separatorPos < strRespo.length() - 2)
    {
        respoBuff = buildErrorResponse(500);
        return;
    }
    else
    {
        contLength = respoBuff.size();
        // strRespo = successLine+ "Server: WebServer\r\n" + strRespo;
        if (contLength < GIGABYTE)
        {   
            contLength = contLength - (separatorPos + 2);
            responseStream << successLine
                            << "Content-Length: " << contLength << "\r\n";
            respoHeaders = responseStream.str();   
            strRespo = respoHeaders + strRespo;

        }
        else
            sendChuckedResponse_CGI();
        std::vector <char> tmp(strRespo.begin(), strRespo.end());
        respoBuff = tmp;
    }
}

int cgiHandling::generateResponse(int sv, std::vector <char> &respoBuff)
{
    char buff[BUFFER_SIZE];
    int rd;
    respoBuff.clear();
    while (1)
    {
        rd = recv(sv, buff, BUFFER_SIZE - 1, 0);
        buff[rd]='\0';
        respoBuff.insert(respoBuff.end(), buff, buff + rd);
        if (rd == 0)
            break;
    }

    std::cout << "Response Readed from the child CGI :\n";            
    std::cout << "Now Generating a proper one :\n";            
    buildProperReponse(respoBuff);
    // printVecChar(respoBuff);
    return 0;
}


void cgiHandling::getFullScriptPath(BaseNode *root, HttpRequest &req, std::string &p)
{
    DirectoryListing obj;
        
    std::map <std::string , std::string> :: iterator it = req.headers.find("Host");; 
    // if (it == req.headers.end())
    // {
    //     exit (1);
    // }
    std::string hostFound = it->second;
    fillReqStruct(root, obj, req.uri, req.headers.at("Host") );
    
    scriptFullPath = obj.getRoot() + req.uri;
    p = obj.getRoot() + req.uri;
    // std::cout << GREEN << "FOUND this Host : " << hostFound + "\n"
    //         << "found this full Script PAth :" << scriptFullPath  <<"\n"
    //         << "path Addr :" << &scriptFullPath   << "\n"
    //         <<   "this obj addr:" << this   
    // << DEF<< "\n" ;
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

