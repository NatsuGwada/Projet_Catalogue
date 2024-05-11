#include <stdio.h>
#include <string.h>
#include <zip.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_WORDLIST_FILENAME_LENGTH 256
#define MAX_PASSWORD_LENGTH 128
#define MAX_WORD_LENGTH 128

// Fonction pour extraire les fichiers d'un zip
void extractFiles(const char *zipFilename, const char *password) {
    zip_t *archive = zip_open(zipFilename, ZIP_RDONLY, NULL);
    if (!archive) {
        fprintf(stderr, "Impossible d'ouvrir l'archive zip\n");
        return;
    }

    int numEntries = zip_get_num_entries(archive, 0);
    for (int i = 0; i < numEntries; ++i) {
        const char *name = zip_get_name(archive, i, 0);
        zip_file_t *file = zip_fopen_encrypted(archive, name, ZIP_RDONLY, password);
        if (file) {
            FILE *outfile = fopen(name, "wb");
            if (!outfile) {
                fprintf(stderr, "Impossible de créer le fichier %s\n", name);
                zip_fclose(file);
                continue;
            }

            char buffer[1024];
            zip_int64_t bytesRead;
            while ((bytesRead = zip_fread(file, buffer, sizeof(buffer))) > 0) {
                fwrite(buffer, 1, bytesRead, outfile);
            }

            fclose(outfile);
            zip_fclose(file);
        }
    }

    zip_close(archive);
}

int main() {
    char zipFilename[MAX_FILENAME_LENGTH];
    char wordlistFilename[MAX_WORDLIST_FILENAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char word[MAX_WORD_LENGTH];
    char choice;

    // Demander à l'utilisateur le nom du fichier zip
    printf("Entrez le nom du fichier zip : ");
    scanf("%s", zipFilename);

    // Demander à l'utilisateur le nom du fichier wordlist
    printf("Entrez le nom du fichier wordlist : ");
    scanf("%s", wordlistFilename);

    // Ouvrir l'archive zip spécifiée par l'utilisateur
    zip_t *archive = zip_open(zipFilename, ZIP_RDONLY, NULL);
    if (!archive) {
        fprintf(stderr, "Impossible d'ouvrir l'archive zip\n");
        return 1;
    }

    // Ouvrir le fichier wordlist
    FILE *wordlistFile = fopen(wordlistFilename, "r");
    if (!wordlistFile) {
        fprintf(stderr, "Impossible d'ouvrir le fichier wordlist\n");
        zip_close(archive);
        return 1;
    }

    // Lire chaque mot de passe de la wordlist
    while (1) {
        rewind(wordlistFile); // Revenir au début du fichier
        while (fgets(word, sizeof(word), wordlistFile) != NULL) {
            // Supprimer le caractère de nouvelle ligne si présent
            word[strcspn(word, "\n")] = '\0';
            // Essayer d'extraire les fichiers du zip avec ce mot de passe
            zip_file_t *file = zip_fopen_encrypted(archive, "password.txt", ZIP_RDONLY, word);
            if (file) {
                printf("Mot de passe trouvé : %s\n", word);
                // Extraire les fichiers du zip avec le mot de passe correct
                extractFiles(zipFilename, word);
                zip_fclose(file);
                zip_close(archive);
                fclose(wordlistFile);
                return 0;
            }
        }
        printf("Aucun mot de passe trouvé dans la wordlist.\n");
        printf("Voulez-vous continuer à chercher dans la wordlist ? (y/n) : ");
        scanf(" %c", &choice);
        if (choice != 'y') {
            break;
        }
    }

    fclose(wordlistFile);
    zip_close(archive);

    return 0;
}
