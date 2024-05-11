#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

#define MAX_FILENAME_LENGTH 128
#define MAX_PASSWORD_LENGTH 128

int compare_with_wordlist(const char *zip_path, const char *wordlist_path) {
    // Open your zip
    int err = 0;
    zip_t *zipfile = zip_open(zip_path, 0, &err);
    if (zipfile == NULL)
    {
        fprintf(stderr, "Error, cannot open ZIP file.\n");
        return 1;
    }

    int entities = zip_get_num_entries(zipfile, 0);
    if (entities < 0)
    {
        fprintf(stderr, "Error 404.\n");
        zip_close(zipfile);
        return 1;
    }

    // Open wordlist file
    FILE *wordlist_file = fopen(wordlist_path, "r");
    if (wordlist_file == NULL)
    {
        fprintf(stderr, "Error, cannot open wordlist file.\n");
        zip_close(zipfile);
        return 1;
    }

    // Cycle through each entry in the ZIP file
    for (int i = 0; i < entities; i++)
    {
        struct zip_stat stockage;
        zip_stat_index(zipfile, i, 0, &stockage);

        // Open 
        zip_file_t *filezip = zip_fopen_index(zipfile, i, 0);
        if (filezip == NULL)
        {
            fprintf(stderr, "Error during opening the door %d in zip file.\n", i);
            zip_close(zipfile);
            fclose(wordlist_file);
            return 1;
        }
        FILE *output_file = fopen(stockage.name, "wb");
        if (output_file == NULL)
        {
            fprintf(stderr, "Error creating output file for input %d.\n", i);
            zip_fclose(filezip);
            zip_close(zipfile);
            fclose(wordlist_file);
            return 1;
        }
        char buf[1024];
        zip_int64_t bytes;
        while ((bytes = zip_fread(filezip, buf, sizeof(buf))) > 0)
        {
            fwrite(buf, 1, bytes, output_file);
        }
        
        fclose(output_file);
        zip_fclose(filezip);

        // Compare with wordlist
        char word[256];
        rewind(wordlist_file); // Go back to the beginning of the wordlist
        while (fscanf(wordlist_file, "%s", word) != EOF) {
            if (strcmp(word, stockage.name) == 0) {
                printf("Successfull\n");
                zip_close(zipfile);
                fclose(wordlist_file);
                return 0;
            }
        }
    }

    // Close the ZIP folder
    zip_close(zipfile);
    fclose(wordlist_file);

    printf("Decompression completed successfully !\n");

    return 0;
}

int main(void)
{
    char zip_path[MAX_FILENAME_LENGTH];
    char wordlist_path[MAX_FILENAME_LENGTH];

    // Ask for ZIP path
    printf("Write your ZIP path: ");
    scanf("%s", zip_path);

    // Ask for wordlist path
    printf("Write your wordlist path: ");
    scanf("%s", wordlist_path);

    // Compare with wordlist
    if (compare_with_wordlist(zip_path, wordlist_path) == 0) {
        return 0; // Success
    }

    return 1; // Failure
}
