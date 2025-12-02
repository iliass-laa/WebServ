#include "../../headers/webserver.hpp"
#include "../../../req-res/response.hpp"

void printThingsToCheck(DirectoryListing &obj)
{
    std::cout   << GREEN 
                << "root :" << obj.getRoot() 
                << "\nAutoIndex :" << obj.getAutoIndex()
                << "\nMax BodySize :" << obj.getMaxBodySize()
                << "\nHas Index File:"<< obj.getHasIndexFile()
                << DEF << "\n";
}

std::size_t  translateMBS(std::string val)
{
    std::string unit;
    std::size_t res;
    std:: map <char , std::size_t> scaler;

    scaler.insert ( std::pair<char,std::size_t>('k',1024) );
    scaler.insert ( std::pair<char,std::size_t>('m',1048576) );
    scaler.insert ( std::pair<char,std::size_t>('g',1073741824) );

    res = std::strtoul(val.c_str(), NULL,10);
    if (isalpha(val[val.length()-1]))
        res = res * scaler.at(val[val.length()-1]);
    // std::cout <<BLUE << "this is the Max Body Size : "
    //     << res << DEF<<"\n";
    return res;
} 

std::size_t checkForMaxBodySize(ContextNode *cNode){
    DirectiveNode *dNode;
    std::size_t  ret = DEF_MAX_BODY_SIZE; 
    for (int i = 0;i < cNode->nbrChilds;i++)
    {
        if (cNode->Childs[i]->typeNode == isDirective)
        {
            dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
            if (!dNode->key.compare("client_max_body_size"))
                ret = translateMBS(dNode->value.back());
        }
    }
    if (ret <= 0)
        ret = DEF_MAX_BODY_SIZE;
    return ret;
}

void checkDirectiveChilds(ContextNode *cNode, DirectoryListing &obj)
{
    DirectiveNode *dNode;
    std::vector <std::string> allowMeth;
    // std::string co[6] = {"MainContext","EventsContext", "HttpContext", "TypesContext", "ServerContext", "LocationContext" };

    // std::cout << RED << "Checking Directive for this Context:" <<co[cNode->type_Context]
    //             <<DEF << "\n";
    if (!cNode)
        return;
    for (int i = 0;i < cNode->nbrChilds;i++)
    {
        if (cNode->Childs[i]->typeNode == isDirective)
        {
            dNode = dynamic_cast<DirectiveNode *>(cNode->Childs[i]);
            if (!dNode->key.compare("autoindex")&& !dNode->value.back().compare("on"))
                obj.setAutoIndex(true);
            else if (!dNode->key.compare("index")&& dNode->value.size() != 0)
                obj.setIndexFile(dNode->value);
            else if (!dNode->key.compare("root"))
                obj.setRoot(dNode->value.back());
            else if (!dNode->key.compare("limit_except"))
                obj.setAllowedMethods(dNode->value);
        }
    }
    // std::cout << "Root :" << obj.getRoot() << "\n";
}

void fillReqStruct(BaseNode*root, DirectoryListing &obj, std::string uri, std::string host)
{
    ContextNode *cNode, *serverNode, *httpNode;
    std::string port, serverName;
    std::vector <std::string> allowMethDef ;
    allowMethDef.push_back("GET");
    allowMethDef.push_back("POST");
    allowMethDef.push_back("DELETE");
    obj.setAllowedMethods(allowMethDef);

    // std::cout <<PINK << "URI :"<<uri
    //             << "| HOST : "<< host
    //             <<"\n"<<DEF ;

    int iport;
    serverName = host.substr(0, host.find(":"));
    port = host.substr(host.find(":")+ 1,host.length());


    httpNode = findHttpContext(root);
    // if(!httpNode)
    //     throw(ConfigFileError("No http Found"));
    iport = std::atoi(port.c_str());
    // if (iport < 0)
    //     throw(ConfigFileError("invalid Port"));
    serverNode = findServerContext(httpNode, serverName,iport);
    // if(!serverNode)
    //     throw(ConfigFileError("No Server Found"));
    checkDirectiveChilds(serverNode, obj);
    cNode = findLocationContext(serverNode, uri);
    // if (!cNode)
    //     throw(ConfigFileError("No Location Found"));
    // std::cout <<PINK << "VAL of Context:"<<cNode->val.back()<<"\n"<<DEF ;
    // if (!cNode)
    checkDirectiveChilds(cNode, obj);
    // obj.setDefault(true);
    std::vector <std::string> defIndex;
    if (obj.getHasIndexFile() == false)
    {
        defIndex.push_back("index.html");
        obj.setIndexFile(defIndex);
    }
    // std::cout <<YELLOW 
    //         << "what does the fill  Req Res did:\n"
    //         << "Root :" <<obj.getRoot() << "\n"
    //         << "index :" << obj.getHasIndexFile()
    //         << "\n"<<DEF;
    // printThingsToCheck(obj);
 }


void getMaxBodySize(BaseNode *root, size_t &mxBdSz, std::string host)
{
    ContextNode *httpNode, *serverNode;
    std::string serverName, port;
    std::size_t tmp;
    int iport;

    serverName = host.substr(0, host.find(":"));
    port = host.substr(host.find(":")+ 1,host.length());
    httpNode = findHttpContext(root);
    mxBdSz = checkForMaxBodySize(httpNode);
    iport = std::atoi(port.c_str());
    serverNode = findServerContext(httpNode, serverName,iport);
    tmp = checkForMaxBodySize(serverNode);
    if (tmp != DEF_MAX_BODY_SIZE)
        mxBdSz = tmp;    
    // std:: cout << PINK << "> Max Body Size :" << mxBdSz << DEF << "\n";
}