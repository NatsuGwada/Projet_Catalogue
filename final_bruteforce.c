#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

// Structure pour un noeud de la liste
struct Node {
    char password[256];
    struct Node *next;
};

// Fonction pour ajouter un mot de passe à la liste
void addPasswordToList(struct Node **head, const char *password) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour le nouveau noeud.\n");
        exit(1);
    }
    strcpy(newNode->password, password);
    newNode->next = *head;
    *head = newNode;
}

// Fonction pour libérer la mémoire occupée par la liste
void freeList(struct Node *head) {
    struct Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    char zip_path[256];
    printf("Entrez le chemin de votre fichier ZIP : ");
    scanf("%255s", zip_path);

    // Ouvrir le fichier ZIP
    int err = 0;
    zip_t *z = zip_open(zip_path, 0, &err);
    if (z == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier ZIP.\n");
        return 1;
    }

    FILE *wordlist_file;
    char txt_path[128];
    printf("Entrez le chemin de votre fichier de wordlist : ");
    scanf("%127s", txt_path);

    // Ouvrir la wordlist
    wordlist_file = fopen(txt_path, "r");
    if (wordlist_file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture de la wordlist.\n");
        zip_close(z);
        return 1;
    }

    // Initialiser la liste
    struct Node *passwordList = NULL;

    // Lire chaque mot de passe de la wordlist et l'ajouter à la liste
    char password[256];
    while (fscanf(wordlist_file, "%255s", password) != EOF) {
        addPasswordToList(&passwordList, password);
    }

    // Fermer la wordlist
    fclose(wordlist_file);

    // Essayer chaque mot de passe de la liste
    struct Node *current = passwordList;
    int found = 0;
    while (current != NULL) {
        zip_t *zip_with_password = zip_open(zip_path, ZIP_CHECKCONS | ZIP_RDONLY, &err);
        if (zip_with_password == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier ZIP avec le mot de passe \"%s\".\n", current->password);
            struct Node *temp = current;
            current = current->next;
            free(temp);
            continue;
        }

        zip_set_default_password(zip_with_password, current->password);

        // Essayer d'extraire les fichiers avec le mot de passe
        int num_entries = zip_get_num_entries(zip_with_password, 0);
        if (num_entries < 0) {
            fprintf(stderr, "Erreur lors de la récupération du nombre d'entrées dans le fichier ZIP.\n");
            zip_close(zip_with_password);
            struct Node *temp = current;
            current = current->next;
            free(temp);
            continue;
        }

        int i;
        for (i = 0; i < num_entries; i++) {
            struct zip_stat st;
            zip_stat_index(zip_with_password, i, 0, &st);
            zip_file_t *f = zip_fopen_index(zip_with_password, i, 0);
            if (f == NULL) {
                fprintf(stderr, "Erreur lors de l'ouverture de l'entrée %d dans le fichier ZIP avec le mot de passe \"%s\".\n", i, current->password);
                zip_close(zip_with_password);
                struct Node *temp = current;
                current = current->next;
                free(temp);
                break;
            }
            FILE *output_file = fopen(st.name, "wb");
            if (output_file == NULL) {
                fprintf(stderr, "Erreur lors de la création du fichier de sortie pour l'entrée %d avec le mot de passe \"%s\".\n", i, current->password);
                zip_fclose(f);
                zip_close(zip_with_password);
                struct Node *temp = current;
                current = current->next;
                free(temp);
                break;
            }
            char buf[1024];
            zip_int64_t nbytes;
            while ((nbytes = zip_fread(f, buf, sizeof(buf))) > 0) {
                fwrite(buf, 1, nbytes, output_file);
            }
            fclose(output_file);
            zip_fclose(f);
        }

        // Si tous les fichiers ont été extraits avec succès, marquer le mot de passe comme trouvé et sortir de la boucle
        if (i == num_entries) {
            printf("Décompression réussie avec le mot de passe : %s\n", current->password);
            found = 1;
            break;
        }

        current = current->next;
    }

    // Libérer la mémoire occupée par la liste restante
    freeList(current);

    // Fermer le fichier ZIP
    zip_close(z);

    if (!found) {
        printf("Aucun mot de passe valide trouvé dans la wordlist.\n");
        return 1;
    }

    return 0;
}
