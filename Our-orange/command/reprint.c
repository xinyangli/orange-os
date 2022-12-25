#include "stdio.h"

int main(int argc, char * argv[])
{
	int i = 0;
	int num = 0;
	char* s = argv[argc - 1];
	while(s[i]!='\0'){
		num*=10;
		num+=s[i]-'0';
		i++;
	}
	for (i = 0; i < num; i++)
		printf("%s", argv[1]);
	printf("\n");
	return 0;
}
