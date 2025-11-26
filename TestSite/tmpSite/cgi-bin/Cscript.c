#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 1024

#include <stdlib.h>

char *strjoin(char *s1, const char *s2) {
    size_t len1 ;
    if (s1)
    	len1= strlen(s1);
    else
	len1 = 0;
	
    size_t len2 = strlen(s2);
    char *res = malloc(len1 + len2 + 1);
    if (!res)
	return NULL;
    if (s1)
    	memcpy(res, s1, len1);
    memcpy(res + len1, s2, len2);
    res[len1 + len2] = '\0';
	free(s1);
    return res;
}

struct info{
	char *f_name, *l_name;
	int age;
} ;

int main(int ac, char **av,char **env)
{
	char *bounderies;
	char buff[BUFF_SIZE], ctHeader[]= "Content-Type: text/html\n\n";
	struct info in;
	char *str = NULL;
	int rd;

	while (1)
	{
		rd = read(0, buff, BUFF_SIZE -1 );
		if (rd <= 0 )
			break;
		buff[rd] = 0;
		str = strjoin(str, buff);
		if (rd != BUFF_SIZE - 1)
			break;	
	}
	in.f_name = strstr(str, "name=\"fname\"");
	if (in.f_name)
	{
		printf(in.l_name);
		printf("\n");
	}
	in.l_name = strstr(str, "name=\"lname\"");
	if (in.l_name)
	{
		printf(in.l_name);
		printf("\n");
	}

	in.l_name = strstr(str, "OLDPWD");
	if (in.l_name)
	{
		printf(in.l_name);
		printf("\n");
	}
	// printf(str);
	free(str);
	return 0;
}
