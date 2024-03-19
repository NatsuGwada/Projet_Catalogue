/*
    Program Name: bruteforce_zip.c
    Description: This program executes a bruteforce attack using a wordlist on an 
    encrypted zip file.
    Author: Gordibus
    Date: 03/16/2024
    License: <License Type, e.g., MIT, GPL, etc.>

    This program is distributed under the <License Type> license.
    You should have received a copy of the license with this file.
    If not, see <Link to License>.

    For more information, see the README.md file or visit <Link to your GitHub project>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

// Structure for list
struct Node
{
    char token[256];
    struct Node *next;
};

// Function to add a token to the list
void add_token_to_list(struct Node **head, const char *token)
{
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error allocating memory for the new node.\n");
        exit(1);
    }
    strcpy(new_node->token, token);
    new_node->next = *head;
    *head = new_node;
}

// Function to free the memory occupied by the list
void free_list(struct Node *head)
{
    struct Node *temp;
    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main(void)
{
    // Limit size to 256 characters
    FILE *wordlist;
    char zip_path[256];
    char wordlist_path[256];
    char token[256];

    printf("Enter your ZIP file path: ");
    scanf("%255s", zip_path); // Limit size to 255 characters to prevent buffer overflow

    // Open the ZIP file
    int error = 0;
    zip_t *z = zip_open(zip_path, 0, &error);
    if (z == NULL)
    {
        fprintf(stderr, "Error opening the ZIP file.\n");
        return 1;
    }
    printf("Enter your wordlist file path: ");
    scanf("%255s", wordlist_path);

    wordlist = fopen(wordlist_path, "r");
    if (wordlist == NULL)
    {
        fprintf(stderr, "Error opening the wordlist file.\n");
        zip_close(z);
        return 1;
    }
    // Initialize the token list
    struct Node *token_list = NULL;

    while (fscanf(wordlist, "%255s", token) != EOF)
    {
        add_token_to_list(&token_list, token);
    }

    // Close the wordlist
    fclose(wordlist);

    // Try every token in the list
    struct Node *current = token_list;
    int found = 0;
    while (current != NULL)
    {
        zip_t *zip_with_token = zip_open(zip_path, ZIP_CHECKCONS | ZIP_RDONLY, &error);
        if (zip_with_token == NULL)
        {
            fprintf(stderr, "Error opening the ZIP file with token \"%s\".\n", current->token);
            current = current->next;
            continue;
        }
        zip_set_default_password(zip_with_token, current->token);

        // Try to extract files with the token
        int num_entries = zip_get_num_entries(zip_with_token, 0);
        if (num_entries < 0)
        {
            fprintf(stderr, "Error retrieving the number of entries in the ZIP file.\n");
            zip_close(zip_with_token);
            current = current->next;
            continue;
        }
        int i;
        for (i = 0; i < num_entries; i++)
        {
            struct zip_stat st;
            zip_stat_index(zip_with_token, i, 0, &st);
            zip_file_t *f = zip_fopen_index(zip_with_token, i, 0);
            if (f == NULL)
            {
                fprintf(stderr, "Error opening entry %d in the ZIP file with token \"%s\".\n", i, current->token);
                zip_close(zip_with_token);
                break;
            }
            FILE *output_file = fopen(st.name, "wb");
            if (output_file == NULL)
            {
                fprintf(stderr, "Error creating the output file for entry %d with token \"%s\".\n", i, current->token);
                zip_fclose(f);
                zip_close(zip_with_token);
                break;
            }
            char buf[1024];
            zip_int64_t nbytes;
            while ((nbytes = zip_fread(f, buf, sizeof(buf))) > 0)
            {
                fwrite(buf, 1, nbytes, output_file);
            }
            fclose(output_file);
            zip_fclose(f);
        }

        // Close the ZIP file with the token
        zip_close(zip_with_token);

        // If all files were successfully extracted, mark the token as found and break out of the loop
        if (i == num_entries)
        {
            printf("Decompression successful with token: %s\n", current->token);
            found = 1;
            break;
        }

        current = current->next;
    }

    // Free the memory occupied by the list
    free_list(token_list);

    // Close the ZIP file
    zip_close(z);

    if (!found)
    {
        printf("No valid token found in the wordlist, try with an other wordlist men...\n");
        return 1;
    }

    return 0;
}
