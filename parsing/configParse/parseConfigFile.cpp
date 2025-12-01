#include "../headers/webserver.hpp"
#include "../configParse/checkTree.hpp"


int getNextQ(std::string line, int start, int Quote)
{
    start++;
    while (line[start] && line[start] != Quote)
        start++;
    return start;
}

void printTokenz(TokenizerData &t)
{   
    std::cout << "SIZE OF the TOKENS :"<<t.tokens.size() << "\n";
    std::cout << "Those are tokens :\n";
    std::vector <std::string> ::iterator it;
    int i = 0;
    it = t.tokens.begin();
    while(it != t.tokens.end())
    {
        std::cout << i<<"_'" <<*it << "'\n";
        i++;
        it++;
    }
    std::cout << "Those are braquets remaining  : ";
    // std::cout << t.braquets.size() <<  "\n";
    while(!t.braquets.empty())
    {
        std::cout << t.braquets.top() << "< | >";
        t.braquets.pop(); 
    }
}
int isAComment(std::string line)
{
    int i = 0;
    while(isspace(line[i]))
        i++;
    if (line[i] == '#')
        return 1;
    return 0;
}

int isSymbol(char s)
{
    if (s == O_Br || s == SEMI || s == C_Br)
        return 1;
    return 0;
}

int countNbrTokens(std::string line)
{
    int i, res ;

    res = 0;
    i = 0;
    while (line[i])
    {
        if (line[i] == DoubleQ || line[i] == SingleQ)
        {
            res++;
            i = getNextQ(line, i, line[i]);
            if (line[i] == DoubleQ || line[i] == SingleQ)
                i++;
        }
        else{
            if (isSymbol(line[i]))
                res++;
            else if (!isspace(line[i]) && (isspace(line[i + 1]) || line[i + 1] == '\0'|| line[i + 1] == '\n' || isSymbol(line[i + 1])))
                res++;
            i++;
        }
    }
    return res;
}

int getStart(std::string line, int end)
{
    while (isspace(line[end]))
        end++;
    return end;
}



int getEnd(std::string line, int start)
{
    if (isSymbol(line[start]))
        return start + 1;
    if (line[start] == SingleQ ||  line[start] == DoubleQ)
        return getNextQ(line, start, line[start]);
    while (line[start] && !isspace(line[start]) && !isSymbol(line[start]) && line[start] != '\n')
        start++;
    return start;
}

int checkTokens(std::vector <std::string> tokens, TokenizerData &tk, std::string line)
{
    std::size_t i = 0;
    while (i< tokens.size())
    {
        // std::cout << "Comparing this Token :>"<<tokens.at(i) << "<\n";
        if (!tokens.at(i).compare("{"))
        {
            // std::cout << ">>>>>>>>>>>Found { In this line :" + line+" , token Nbr : " << i <<"<<<<<<<<<<\n";
            tk.braquets.push(1);
        }
        else if (!tokens.at(i).compare("}"))
        {
            // tk.braquets.push(2);

            if (tk.braquets.empty())
            {
                // std::cout <<"AT this position i am " <<i << "\n";
                // std::cout <<"AT this line :" + line << "\n";
                return 1;
            }
            tk.braquets.pop();
        }
        i++;
    }
    (void)line;
    return 0;
}

void splitTheLine(std::string line, TokenizerData &tk)
{   
    // std::cout << ">>>Splitting Facts!\n";
    std::vector <std::string> tmpTokens;
    int nbrTokens, end ,start,i;
    end = 0;
    start = 0;
    i = 0;

    while(isspace(line[i]))
        i++;
    line.erase(0, i);
    if (line.length() == 0 ||line.at(0) == '\n')
        return;
    nbrTokens = countNbrTokens(line);
    i = 0;
    // std::cout << ">>Line  :" + line << "\n>>NBR Tokens :" << nbrTokens << "\n";
    while (i < nbrTokens)
    {
        start = getStart(line, end);
        end = getEnd(line, start);
        if (line[start] == SingleQ || line[start] == DoubleQ)
        {
            start++;
        }
        // std::cout <<"i :" << i 
        //         << ", start :" << start
        //         << ", end :" << end  
        //         << "Token :>" <<line.substr(start, end - start)<< "<\n";
        
        tmpTokens.push_back(line.substr(start, end -start));
        i++;
    }
    if(!checkTokens(tmpTokens, tk, line))
        tk.tokens.insert(tk.tokens.end(), tmpTokens.begin(), tmpTokens.end());
    else
    {
        // std::cout << ">>>>>>>>>>i am about to throw !, Extra closing braquets \n";
        throw (ConfigFileError());
    }
}




void tokenizer(std::fstream &f, TokenizerData &tk)
{
    // std::cout << "Tokenizer EnteryPoint\n";
    std::string line;
    
    while (getline(f, line))
    {
        if (!isAComment(line))
        {
            splitTheLine(line, tk);
            // std::cout <<"Tokens in line :"<< tk.tokens.back() << "\n";
        }

    }
}

BaseNode *parseConfigFile(const char *path)
{
    checkTree lastDance;
    TokenizerData tk;
    BaseNode *root;
    std::fstream f(path);
    if (f.fail())
        throw(ConfigFileError("Config File Not Found (Path incorrect)!"));
    tokenizer(f, tk);
    if (!tk.braquets.empty())
    {
        std::cerr <<"A Brquets left open\n";
        throw(ConfigFileError());
    }
    if (tk.tokens.size() == 0)
        throw(ConfigFileError("Config File is Empty ! please Consider Filling it Properly."));
    root = buildTree(tk);
    try 
    {
        lastDance.finalChecking(root);
    }
    catch(std::exception &e)
    {
        std::cerr <<RED << e.what() << " <<\n"<<DEF;
        freeTree(root);
        return NULL;
    }
    return root;
}
