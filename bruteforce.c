#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

/*void success (int *password, int *wordlist)
{

}
void wordlist
{

}
*/

int main(void)//int argc, char *argv[])
{
/*	char path_buffer[100];
	//int wordlist = 
	FILE*file_in_question;
	FILE*file[100]; = NULL;

	file = fopen("wordlist.txt", "r+");

	char input[100];
	printf("Keep");
	
	printf("Grab your wordlist path : ");
	fgets(path_buffer, sizeof(path_buffer), stdin);
*/
	//int file[100];
	FILE*file;
	
	char path[128];
	char token[128];

	printf("Read your wordlist path: ");
	scanf("%s", path);

		if ((file = fopen(path, "rb")) != NULL)
		{
			printf("Your file %s is open.\n", path);

			printf("The 5 first token is: %s\n", path);

			for (int i = 0; i < 5 && fscanf(file, "%s", token) != EOF; ++i)
			{
				printf("%d. %s\n", i + 1, token);
			}
			fclose(file);
		}
		else {
			printf("Error I can't open your file %s.\n", path);
		}
		
	return 0;
}
