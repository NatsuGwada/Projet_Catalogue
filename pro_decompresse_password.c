#include <stdio.h>
#include <stdlib.h>
#include <zip.h>

int main(void)
{
    char zip_path[128];
    char password[128]; // Variable pour stocker le mot de passe
    printf("Write your ZIP path: ");
    scanf("%s", zip_path);

    // Demander le mot de passe si nécessaire
    printf("Enter password (press Enter if no password): ");
    scanf("%s", password);

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

    // Cycle through each entry in the ZIP file
    for (int i = 0; i < entities; i++)
    {
        struct zip_stat stockage;
        zip_stat_index(zipfile, i, 0, &stockage);

        // Open 
        zip_file_t *filezip;
        if (password[0] != '\0') // Vérifier si un mot de passe a été saisi
            filezip = zip_fopen_index_encrypted(zipfile, i, 0, password); // Ouvrir avec mot de passe
        else
            filezip = zip_fopen_index(zipfile, i, 0); // Ouvrir sans mot de passe

        if (filezip == NULL)
        {
            fprintf(stderr, "Error during opening the door %d in zip file.\n", i);
            zip_close(zipfile);
            return 1;
        }
        FILE *output_file = fopen(stockage.name, "wb");
        if (output_file == NULL)
        {
            fprintf(stderr, "Error creating output file for input %d.\n", i);
            zip_fclose(filezip);
            zip_close(zipfile);
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
    }
    // Close the ZIP folder
    zip_close(zipfile);
    
    printf("Decompression completed successfully !\n");
    
    return 0;
}
