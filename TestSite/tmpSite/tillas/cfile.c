#include <stdio.h>


int main(int ac, char **av, char **env)
{
	int i = 0;
	while (env[i])
	{
		printf("from C_file>%s\n",env[i]);
		i++;
	}
}
