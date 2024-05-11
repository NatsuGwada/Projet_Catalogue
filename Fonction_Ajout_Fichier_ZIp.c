void ajouter_fichier_zip(const char *nom_fichier_zip, const char *chemin_vers_fichier_a_ajouter){
    zip_t *archive = zip_open(nom_fichier_zip, ZIP_CREATE, NULL);

    if (!archive) {
        fprintf(stderr, "Erreur : Impossible de crée le fichier '%s'\n ", nom_fichier_zip);
        return;
    }

    int resultat_ajout = zip_file_add(archive, chemin_vers_fichier_a_ajouter, NULL, ZIP_FL_ENC_GUESS);
    if (resultat_ajout < 0){
        fprintf(stderr, "Erreur : Impossible d'ajouter le fichier '%s' à l'archive ZIP '%s'\n", chemin_vers_fichier_a_ajouter, nom_fichier_zip);
        zip_close(archive);
        return;

    }

    printf("Le fichier '%s' a été ajouté avec succès à l'archive ZIP '%s'\n", chemin_vers_fichier_a_ajouter, nom_fichier_zip);

    zip_close(archive);
    
}
