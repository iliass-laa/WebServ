#include "cgi.hpp"

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

    int execRet;
    close(sv[1]);

    getEnvVars(req);
    close(1);
    dup(sv[0]);
    close(0);
    dup(sv[0]);
    std::vector<char*> tmpArgs;
    tmpArgs.push_back(const_cast<char*>(scriptFullPath.c_str()));
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
        }
    }
    close(sv[0]);
    return sv[1];
}

bool validHeaders(std::string str, std::size_t pos)
{
    std::string line ,   nessecaryHeader("Content-Type:");
    std::size_t np,semipos, index;
    index = 0;

    while (true)
    {   
        np = str.find("\n");
        index += np;
        if (np == std::string::npos)
            break;
        line = str.substr(0, np);
        if (index == pos)
            break;
        semipos = line.find(":");
        if (semipos == std::string::npos)
        {
            std::cout << "this line don't have a ':' ::" << line << "<<\n";
            return false;
        }
        str = str.substr(np + 1, str.length());
        index++;
    }
    np = line.find(nessecaryHeader);
    if (np != 0)
        return false;
    return true;
}

void cgiHandling::buildProperReponse(std::vector <char>& respoBuff)
{
    std::string successLine("HTTP/1.1 200 OK\r\n"),respoHeaders, strRespo(respoBuff.begin(), respoBuff.end());
    std::size_t separatorPos, contLength;
    std::ostringstream responseStream;
    
    separatorPos =  strRespo.find("\n\n");

    if (separatorPos == std::string::npos || !validHeaders(strRespo, separatorPos))
    {   
        std::cerr << CYAN << strRespo <<"\n"<< DEF ;

        respoBuff = buildErrorResponse(500);
        return;
    }
    else
    {
        contLength = respoBuff.size();
        contLength = contLength - (separatorPos + 2);
        responseStream << successLine
                        << "Content-Length: " << contLength << "\r\n";
        respoHeaders = responseStream.str();   
        strRespo = respoHeaders + strRespo;
        std::vector <char> tmp(strRespo.begin(), strRespo.end());
        respoBuff = tmp;
    }
}

int cgiHandling::generateResponse(int sv, std::vector <char> &respoBuff)
{
    char buff[BUFFER_SIZE];
    int rd;

    while (1)
    {
    
        memset(buff, 0, BUFFER_SIZE);
        rd = recv(sv, buff, BUFFER_SIZE - 1, 0);

        if (rd >=0)
        {
            buff[rd]='\0';
            respoBuff.insert(respoBuff.end(), buff, buff + rd);
            if (rd == 0)
            {
                break;
            }
        }
        else
        {
            return 1;
        }
    }
           
    buildProperReponse(respoBuff);
    return 0;
}


void cgiHandling::getFullScriptPath(BaseNode *root, HttpRequest &req)
{
    DirectoryListing obj;
    std::map <std::string , std::string> :: iterator it = req.headers.find("Host");; 
    std::string hostFound = it->second;
    fillReqStruct(root, obj, req.uri, req.headers.at("Host") );
    scriptFullPath = obj.getRoot() + req.uri;
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
}

