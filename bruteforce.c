#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

/*void success (int *password, int *wordlist)
{

feat/bruteforce

feat/bruteforce


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
	//Ask to write his path to open and read the texte
	printf("Write your wordlist path:  ");
	scanf("%s", path);
		//Open the file to "read" 
		if ((file = fopen(path, "rb")) != NULL)
		{
// To read all file /!\ You need to remove hooks just above the line

		while (fscanf(file, "%s", token) != EOF)
		{
			printf("%s", token);
			
		}
	char choice[50];
	printf("\n DO you want to see the tokens inside the file ? (Y/N) : ");
	scanf("%c", &choice);

	if (choice == 'y')
	{
		rewind (file);
		while (fscanf(file, "%s", token) != EOF)
		{
			printf("%s", token);
		}
	}
	fclose(file);
	}	else 
	{
		printf("No such file.\n");
	}

	
		/*{
			printf("Your file is open.\n");

			printf("The 5 first token is: \n");

			for (int i = 0; i < 5 && fscanf(file, "%s", token) != EOF; ++i)
			{
				printf("%d. %s\n", i + 1, token);
			}
			fclose(file);
		}
		else {
			printf("Error I can't open your file.\n");
		}*/
		


	return 0;
}
