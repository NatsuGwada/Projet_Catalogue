#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

int main(void)
{
	FILE*file;
	
    int count = 0;
	char path[128];
	char token[128];

	printf("Write your wordlist path: ");
	scanf("%s", path);
		if ((file = fopen(path, "rb")) != NULL)
		{
// To read all file /!\ You need to remove hooks just above the line

		while (fscanf(file, "%s", token) != EOF)
		{
			//printf("Successfull ! %s", token);
			
		}
	char choice;
	printf("\nDo you want to see how much tokens is inside this file ? (y/n) : ");
	scanf(" %c", &choice);

	if (choice == 'y')
	{
		rewind (file);
		
        while (fgets(token, sizeof(token), file) != NULL)
		{
			// To printf all token in txt file
			//printf("%s", token);
            count++;
		}
        printf ("%d token are detected.\n", count);
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
