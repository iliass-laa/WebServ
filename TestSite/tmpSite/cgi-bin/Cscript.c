#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main(int ac, char **av,char **env)
{
	char str[1024];
	printf("Content-type: text/html\n\n");
	int readed = 0;
	int fdFile  = open ("/tmp/file1",O_CREAT | O_TRUNC | O_RDWR , 0644);
	
	printf("<h1>print The what on the STDIN : >> </h1>");
	while (1)
	{
		readed = read(0, str, 1024);
		if (readed != 1024)
		{
			str[readed] = '\0';
			dprintf(fdFile,">>>> : %s||||", str);
			printf(">>>> : %s||||", str);
			close(fdFile);
			break;
		}
		else
			printf("####%s<<<<", str);
	}
	printf(">>>>DONE <<\n");
}
