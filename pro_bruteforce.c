#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_WORDLIST_FILENAME_LENGTH 256
#define MAX_PASSWORD_LENGTH 128
#define MAX_WORD_LENGTH 128


int main(void)
{
	FILE*file;
	
    //int count = 0;
	char txt_path[128];
	char token[128];
	char zip_file[128];
	char choice;

	printf("Write your wordlist path: ");
	scanf("%s", txt_path);
		if ((file = fopen(txt_path, "rb")) != NULL)
		{
// To read all file /!\ You need to remove hooks just above the line
	
	printf("Write your zipfile path: ")
	scanf("%s",zip_file);

	printf("\nDo you want to crack this file ? (y/n) : ");
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
	
	return 0;
}
