#include <stdio.h>
#include <unistd.h>
#include <unistd.h>

#define BS 1024

int main()
{
	char b[BS];
	int rd;
	printf("Content-Type: text/html\n\n");
	while (1)
	{
		rd= read(0, b, BS -1);
		if (rd <= 0)
			break;
		b[rd] = 0;
		printf("%s",b);
		if (rd != BS -1)
			break;
	}
}
