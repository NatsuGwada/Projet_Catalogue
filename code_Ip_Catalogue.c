#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char octets[4];
} ip_address;

typedef struct {
    size_t size;
    ip_address *addresses;
} address_catalog;

address_catalog *create_address_catalog() {
    address_catalog *catalog = malloc(sizeof(address_catalog));
    catalog->size = 0;
    catalog->addresses = NULL;
    return catalog;
}

void add_address_to_catalog(address_catalog *catalog, ip_address address, ip_address mask) {
    catalog->size++;
    catalog->addresses = realloc(catalog->addresses, catalog->size * sizeof(ip_address));
    catalog->addresses[catalog->size - 1] = address;
}

void remove_address_from_catalog(address_catalog *catalog, ip_address address, ip_address mask) {
    int found = 0;
    for (int i = 0; i < catalog->size; i++) {
        if (found) {
            catalog->addresses[i - 1] = catalog->addresses[i];
        } else if (memcmp(&(catalog->addresses[i]), &address, sizeof(ip_address)) == 0) {
            found = 1;
        }
    }
    if (found) {
        catalog->size--;
        catalog->addresses = realloc(catalog->addresses, catalog->size * sizeof(ip_address));
    }
}

address_catalog *filter_addresses_by_mask(address_catalog *catalog, ip_address ip, ip_address mask) {
    address_catalog *filtered_catalog = create_address_catalog();
    for (int i = 0; i < catalog->size; i++) {
        ip_address address = catalog->addresses[i];
        if ((address.octets[0] & mask.octets[0]) == (ip.octets[0] & mask.octets[0]) &&
            (address.octets[1] & mask.octets[1]) == (ip.octets[1] & mask.octets[1]) &&
            (address.octets[2] & mask.octets[2]) == (ip.octets[2] & mask.octets[2]) &&
            (address.octets[3] & mask.octets[3]) == (ip.octets[3] & mask.octets[3])) {
            add_address_to_catalog(filtered_catalog, address, mask);
        }
    }
    return filtered_catalog;
}

void save_catalog_to_file(address_catalog *catalog, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp != NULL) {
        fwrite(&catalog->size, sizeof(size_t), 1, fp);
        fwrite(catalog->addresses, sizeof(ip_address), catalog->size, fp);
        fclose(fp);
    }
}

address_catalog *load_catalog_from_file(const char *filename) {
    address_catalog *catalog = create_address_catalog();
    FILE *fp = fopen(filename, "rb");
    if (fp != NULL) {
        fread(&catalog->size, sizeof(size_t), 1, fp);
        catalog->addresses = malloc(catalog->size * sizeof(ip_address));
        fread(catalog->addresses, sizeof(ip_address), catalog->size, fp);
        fclose(fp);
    }
    return catalog;
}

void print_address(ip_address address) {
    printf("Sous forme decimal : %d.%d.%d.%d", address.octets[0], address.octets[1], address.octets[2], address.octets[3]);
}

void print_binary_address(ip_address address) {
    printf("Sous forme binaire : ");
    for (int i = 0; i < 4; i++) {
        unsigned char octet = address.octets[i];
        int tab[8];
        for (int j = 0; j < 8; j++) {
            tab[j] = octet % 2;
            octet /= 2;
        }
        for (int j = 7; j >= 0; j--) {
            printf("%d", tab[j]);
        }
        if (i != 3) {
            printf(".");
        }
    }
}

void print_hexadecimal_address(ip_address address) {
    printf("Sous forme hexadecimal : %x.%x.%x.%x", address.octets[0], address.octets[1], address.octets[2], address.octets[3]);
}

void is_private_or_public(ip_address address) {
    if (address.octets[0] == 10 || (address.octets[0] == 172 && address.octets[1] >= 16 && address.octets[1] <= 31) || (address.octets[0] == 192 && address.octets[1] == 168 && address.octets[2] >= 1)) {
        printf("L'addresse IP est prive");
    } else if (address.octets[0] == 127 && address.octets[1] == 0 && address.octets[2] == 0 && address.octets[3] == 1) {
        printf("L'addresse IP est speciale");
    } else {
        printf("L'addresse IP est publique");
    }
}

void is_class(ip_address address) {
    if ((address.octets[0] >= 0 && address.octets[0] <= 126)) {
        printf(" et fait partie de la classe A");
    } else if ((address.octets[0] >= 128 && address.octets[0] <= 191)) {
        printf(" et fait partie de la classe B");
    } else if ((address.octets[0] >= 192 && address.octets[0] <= 223)) {
        printf(" et fait partie de la classe C");
    } else if ((address.octets[0] >= 224 && address.octets[0] <= 239)) {
        printf(" et fait partie de la classe D");
    } else if ((address.octets[0] >= 240 && address.octets[0] <= 255)) {
        printf(" et fait partie de la classe E");
    } else {
        printf(" et ne fait partie d'aucune classe");
    }
}

void print_address_catalog(address_catalog *catalog) {
    printf("\nVoici les adresses presentes dans votre catalogue :\n\n");
    for (int i = 0; i < catalog->size; i++) {
        is_private_or_public(catalog->addresses[i]);
        is_class(catalog->addresses[i]);
        printf("\n");
        print_address(catalog->addresses[i]);
        printf("\n");
        print_binary_address(catalog->addresses[i]);
        printf("\n");
        print_hexadecimal_address(catalog->addresses[i]);
        printf("\n\n");
    }
}

void menu() {
    address_catalog *catalog = load_catalog_from_file("adresses.bin");

    int choice = 0;
    while (choice != 5) {
        printf("\nChoisissez ce que vous voulez faire :\n");
        printf("1. Afficher toutes les adresses du catalogue\n");
        printf("2. Ajouter une nouvelle adresse au catalogue\n");
        printf("3. Retirer une adresse du catalogue\n");
        printf("4. Filrer les adresses par masque\n");
        printf("5. Quitter\n");
        printf("Que voulez-vous faire ? : ");

        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                print_address_catalog(catalog);
                break;
            }
            case 2: {
                ip_address address;
                ip_address mask;
                printf("Entrez l'adresse IP que vous souhaitez ajouter sous forme decimal (Exemple : x.x.x.x) : ");
                scanf("%hhu.%hhu.%hhu.%hhu", &address.octets[0], &address.octets[1], &address.octets[2], &address.octets[3]);
                printf("Entrez le masque de sous réseau (Exemple : x.x.x.x): ");
                scanf("%hhu.%hhu.%hhu.%hhu", &mask.octets[0], &mask.octets[1], &mask.octets[2], &mask.octets[3]);
                add_address_to_catalog(catalog, address, mask);
                save_catalog_to_file(catalog, "adresses.bin");
                printf("Votre adresse a bien etait ajoute au catalogue\n");
                break;
            }
            case 3: {
                ip_address address;
                ip_address mask;
                printf("Entrez l'adresse IP que vous souhaitez supprimer sous forme decimal (Exemple : x.x.x.x) : ");
                scanf("%hhu.%hhu.%hhu.%hhu", &address.octets[0], &address.octets[1], &address.octets[2], &address.octets[3]);
                remove_address_from_catalog(catalog, address, mask);
                save_catalog_to_file(catalog, "adresses.bin");
                printf("L'adresse a bien ete retire du catalogue\n");
                break;
            }
            case 4: {
                ip_address address;
                ip_address mask;
                printf("Entrez votre votre addresse IP : (Exemple : x.x.x.x): ");
                scanf("%hhu.%hhu.%hhu.%hhu", &address.octets[0], &address.octets[1], &address.octets[2], &address.octets[3]);
                printf("Entrez votre filtre sous forme decimal (Exemple : x.x.x.x): ");
                scanf("%hhu.%hhu.%hhu.%hhu", &mask.octets[0], &mask.octets[1], &mask.octets[2], &mask.octets[3]);
                print_address_catalog(filter_addresses_by_mask(catalog, address, mask));
                break;
            }
            case 5: {
                printf("\nA bientot !\n");
                break;
            }
            default:
                printf("\nChoix invalide, veuillez reassayer.\n");
                break;
        }
    }

    free(catalog->addresses);
    free(catalog);
}

int main() {
    menu();
    return 0;
}
