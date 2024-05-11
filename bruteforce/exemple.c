#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>


// To create a new txt file with the right password inside
int main(int argc, char *argv[])
{
FILE*f;
char path[128];
	printf("Write your path: ");
	scanf("%s", path);
	if ( (f = fopen("%s", "w")) !=NULL);
	else 
		printf("An error are detected to create the folder.\n");
	return 0;
}
