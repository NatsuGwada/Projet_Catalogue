
void afficher_aide(){
    printf("Usage: ./zip [Option]... [Fichier]... \n");
    printf("Option:\n");
    printf(" -h , --help voir l'aide\n");
    printf(" -o , --open  Ouvrir un fichier zip\n");
    printf(" -b , --bruteforce  casser le mot de passe avec le dictionnaire\n");
    printf(" -p , --password=PASSWORD utiliser le mot de passe\n");
    printf(" -e , --extract=FILE extraire le fichier\n");
    printf(" -i , --include=FILE inclure le fichier\n");
}

