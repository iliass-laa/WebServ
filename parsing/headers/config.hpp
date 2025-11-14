# ifndef CONFIG_HPP
# define CONFIG_HPP

#include "includes.hpp"

#define O_Br '{'
#define C_Br '}'
#define SEMI ';'
#define SingleQ '\''
#define DoubleQ '"'

#define GIGABYTE 1073741824
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define DEF_MAX_BODY_SIZE  GIGABYTE 



class TokenizerData
{
    public :
        TokenizerData(){};
        ~TokenizerData(){};

        std::stack <int> braquets;
        std::vector <std::string> tokens;
};



int isSymbol(char s);
// BaseNode *buildTree(TokenizerData &tk);




#endif


/*

    server{
        listen : 84
    }

*/