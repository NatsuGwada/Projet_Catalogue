#include <getopt.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <locale.h>

// Déclaration de la variable zip_file à l'extérieur de la fonction main
struct zip *zip_file = NULL;

// we define the name of the directory where the files will be extracted to
#define EXTRACT_DIR "./extracted_files/"

// ascii art
void ascii_art() {
  printw("  #####  ######  #     #   #####   #####         #####\n");
  printw("  #      #    #  ##   ##   #   #   #    #        #    \n");
  printw("  #      #    #  # # # #   #####   #####  #####  #    \n");
  printw("  #      #    #  #  #  #   #       #   #         #    \n");
  printw("  #####  ######  #     #   #       #    #        #####\n");
  printw("\n");
}

// print choices
void print_choices(char *choices[], int max_choice, int choice) {
  printw("Choose an option:\n");
  for (int i = 0; i < max_choice; ++i) {
    if (choice - 1 == i)
      printw("[ * ]");
    else
      printw("[   ]");
    printw("%s\n", choices[i]);
  }
}

// returns the number of the files in the zip
int get_number_files(struct zip *zip_file) {
  int num_files = zip_get_num_entries(zip_file, 0);
  if (num_files < 0) {
    printw("Error getting number of files in zip file\n");
    return 1;
  }
  return num_files;
}

// get the files in the zip
int get_file_names(struct zip *zip_file, int num_files, char *file_names[]) {
  for (int i = 0; i < num_files; ++i) {
    const char *name = zip_get_name(zip_file, i, 0);
    if (!name) {
      printw("Error getting name of file %d\n", i);
    } else {
      // we cant do file_print_choices[i] = name;
      // so we allocate memory for the name of the file because it is a const
      size_t len = strlen(name);
      file_names[i] = malloc(len + 1);
      strcpy(file_names[i], name);
    }
  }
  return 0;
}

// prints the file names and asks the user wich one to print out
void get_file_content(struct zip *zip_file, int file_index) {
  file_index = file_index - 1;
  // Open the specified file
  struct zip_file *file = zip_fopen_index(zip_file, file_index, 0);
  if (!file) {
    printw("Error opening file %d\n", file_index);
    return;
  }
  // Get the size of the file to print a message if empty
  struct zip_stat file_stat;
  zip_stat_index(zip_file, file_index, 0, &file_stat);
  printw("\n");
  if (file_stat.size == 0) {
    printw("This file is empty\n");
    return;
  } else {
    // Print the content of the file
    char *buffer = malloc(file_stat.size);
    int len;
    while ((len = zip_fread(file, buffer, file_stat.size)) > 0) {
      fwrite(buffer, 1, len, stdout);
    }
    printw("\n");
  }
  // Close the file
  zip_fclose(file);
}

// extracting a file from the zip to a directory (./extracted_files/)
void extract_file(struct zip *zip_file, int file_index, char *file_name) {
  file_index = file_index - 1;
  // Open the specified file
  struct zip_file *file = zip_fopen_index(zip_file, file_index, 0);
  if (!file) {
    printw("Error opening file %d\n", file_index);
    return;
  }
  // prepare the path to extract the file to
  char path[256];
  snprintf(path, sizeof(path), "%s/%s", EXTRACT_DIR, file_name);
  // create a new file with the same name in the new directory
  FILE *new_file = fopen(path, "wb");
  if (!new_file) {
    printw("Error creating file %s\n", path);
    return;
  }
  // add the content of the file to the new file
  char buffer[1024];
  int len;
  while ((len = zip_fread(file, buffer, sizeof(buffer))) > 0) {
    fwrite(buffer, 1, len, new_file);
  }
  printw("\nFile %s extracted successfully to %s\n", file_name, EXTRACT_DIR);
  // Close the files
  fclose(new_file);
  zip_fclose(file);
  printw("\n");
}

// ajouter un fichier a l'archive ouverte

int ajout_fichier_au_zip(struct zip *zip_file, const char *file_path) {

  //ouvrir le fichier a ajouter
  FILE *fichier_a_ajouter = fopen(file_path, "rb");
  if (!fichier_a_ajouter) {
    printw("Error , File cannot open\n");
    return 1;
  }
  // prend le là ou il se trouve
  const char *nom_du_ficher = strrchr(file_path, '/');
  if (!nom_du_ficher) {
    nom_du_ficher = file_path;
  } else {
    nom_du_ficher++;
  }


  // Créer une source pour le fichier à ajouter
  struct zip_source *source = zip_source_file(zip_file, file_path, 0, 0);
  if (!source) {
    printw("Cant create source for the file :%s\n", file_path);
    fclose(fichier_a_ajouter);
    return 1;
  }
  // ajoute ton fichier dans le zip

  int index = zip_file_add(zip_file, nom_du_ficher, source, ZIP_FL_ENC_UTF_8);
  if (index < 0 ){
    printw("error cannot add this file :%s\n", file_path);
    fclose(fichier_a_ajouter);
    zip_source_free(source);
    return 1;
  }

  printw("File %s has added to the zip", file_path);


  //fèmé tout

  fclose(fichier_a_ajouter);
  zip_source_free(source);


  return 0;

}

// print the help message
void print_help(const char *program_name) {
  printf("Usage : %s [OPTIONS]\n", program_name);
  printf("Options : \n");
  printf("    -a <ton fichier> ça chabin c'est pour ajouté ton fichier au zip\n");
  printf("    -f <filename> Specify the input file\n");
  printf("    -h Show this help\n");
}


// main function
int main(int argc, char *argv[]) {
  // getopt
  int opt;
  // zip file name, set to NULL by default and set by the user with -f
  const char *filename = NULL;


  // parse the options
  while ((opt = getopt(argc, argv, "af:h")) != -1) {
    switch (opt) {
    case 'f':
      filename = optarg;
      break;

    case 'a': 
      if (filename == NULL) {
        printw("Awa bay la vwa si dokiman aw épi opsyon -f prèmyé tan ti misye \n");
        exit(EXIT_FAILURE);
      }
      if (ajout_fichier_au_zip(zip_file, optarg) != 0) {
        printw(" Cant add file like this !\n\n ");
        printw("Non zanmi dokiman pa ka add kon sa \n");
        exit(EXIT_FAILURE);
      }
      printw(" Add file %s on the ZIP , successfuly !\n\n", optarg);
      printw("OOuuuiii fanmi la dokiman %s aw adan zip la \n!", optarg);

      exit(EXIT_SUCCESS);
      break;

    case 'h':
      print_help(argv[0]);
      exit(EXIT_SUCCESS);

    case '?':
      printf("Invalid option. Use -h or --help for usage.\n");
      exit(EXIT_FAILURE);
    }
  }
  // check if the filename is set
  if (filename == NULL) {
    printf("Invalid option. Use -h or --help for usage.\n");
    exit(EXIT_FAILURE);
  }
  // Initialize ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  // this is for the main menu
  int choice = 1;
  char *choices[5] = {"Print the content of a file", "Extract a file","Mèt dokiman aw ", "Help / Wiki-aw","Quit"};
  int max_choice = 5;
  // the pressed key
  int key;
  // Open the zip file
  zip_file = zip_open(filename, 0, NULL);
  if (!zip_file) {
    printf("Error opening zip file %s\n", filename);
    return 1;
  }
  // Main program loop
  while (1) {
    // Clear the screen
    clear();
    // Print the ascii art
    ascii_art();
    // Display options
    print_choices(choices, max_choice, choice);
    refresh();
    // Get user input
    key = getch();
    // Handle arrow key presses
    if (key == KEY_UP) {
      choice = (choice == 1) ? max_choice : choice - 1;
    } else if (key == KEY_DOWN) {
      choice = (choice == max_choice) ? 1 : choice + 1;
    } else if (key == 10) { // Enter key pressed
      int file_choice = 1;
      int num_files = get_number_files(zip_file);
      char *file_names[num_files];
      // switch case for all the options
      switch (choice) {
      case 1:
        while (1) {
          clear();
          ascii_art();
          get_file_names(zip_file, num_files, file_names);
          printw("Choose a file to print : \n");
          print_choices(file_names, num_files, file_choice);
          refresh();
          key = getch();
          if (key == KEY_UP) {
            file_choice = (file_choice == 1) ? num_files : file_choice - 1;
          } else if (key == KEY_DOWN) {
            file_choice = (file_choice == num_files) ? 1 : file_choice + 1;
          } else if (key == 10) {
            get_file_content(zip_file, file_choice);
            printw("\nPress any key to go back to the menu\n");
            refresh();
            getch();
            break;
          }
        }
        break;
      case 2:
        while (1) {
          clear();
          ascii_art();
          get_file_names(zip_file, num_files, file_names);
          printw("Choose a file to extract : \n");
          print_choices(file_names, num_files, file_choice);
          refresh();
          key = getch();
          if (key == KEY_UP) {
            file_choice = (file_choice == 1) ? num_files : file_choice - 1;
          } else if (key == KEY_DOWN) {
            file_choice = (file_choice == num_files) ? 1 : file_choice + 1;
          } else if (key == 10) {
            extract_file(zip_file, file_choice, file_names[file_choice - 1]);
            printw("\nPress any key to go back to the menu\n");
            refresh();
            getch();
            break;
          }
        }
        break;

case 3:
    // Ajouter un fichier à l'archive
    // Afficher un message pour demander le chemin du fichier à ajouter
    printw("Entrez le chemin du fichier à ajouter à l'archive : ");
    refresh();

    // Lire le chemin du fichier à ajouter depuis l'utilisateur
    char file_path[256];
    int ch;
    int i = 0;

    while ((ch = getch()) != '\n' && i < 255)
    {
        if (ch == KEY_BACKSPACE || ch == 127)
        {
            // Gestion de la touche de suppression ou du caractère ASCII 127
            if (i > 0)
            {
                // Effacer le caractère précédent de l'affichage et du tableau
                printw("\b \b");
                refresh();
                i--;
            }
        }
        else
        {
            // Afficher le caractère et l'ajouter au tableau
            printw("%c", ch);
            refresh();
            file_path[i++] = ch;
        }
    }
    file_path[i] = '\0';

    // Ajouter le fichier à l'archive
    if (ajout_fichier_au_zip(zip_file, file_path) != 0)
    {
        printw("\n\n l'ajout du fichier dans l'archive n'a pas marché.\n");
    }
    else
    {
        printw("\n oui c'est bon , le fichier s'est ajouté avec succès à l'archive.\n");
        
        // Fermer le fichier zip pour sauvegarder les modifications
        if (zip_close(zip_file) == -1) {
            printw("\n Erreur lors de la fermeture du fichier zip.\n");
        }
        else {
            printw("\n Le fichier zip a été correctement mis à jour.\n");
        }
    }

    printw("\nVous avez entré : %s\n", file_path); // Afficher le chemin saisi

    // Attendre que l'utilisateur appuie sur une touche pour continuer
    printw("\nAppuyez sur n'importe quelle touche pour revenir au menu.\n");
    refresh();
    getch();

    break;


      case 4:
        while (1) {
          clear();
          ascii_art();
          print_help(argv[0]); // Afficher les informations d'aide
          printw("Bon chef, le premier choix c'est pour ecrire dans ton fichier.\n\n"
          "le 2eme choix , tu choisis ton fichier a extrait dans le dossier extract_files de ton path.\n\n"
          "le 3eme choix c'est pour mettre un fichier dans le zip(format.txt stp pour debuter)\n\n"
          "le 4eme choix tu es deja la, cest ton wiki pour comprendre comment ici fonctionne.\n\n"
          "le 5eme c'est pour quitter donc au cas ou déja , A pli ta !\n\n"
          "Bon tu connais déja sé nou mèm amuse toi bien avec notre programme. \U0001F918 BigUp 971-972 \U0001F918\n\n");

          printw("\nPress any key to go back to the menu\n");
          refresh();
          getch();
          break;
        }
      break;

      case 5:
        printw("Quitting...\n");
        refresh();
        // Fermer le fichier zip avant de quitter
        if (zip_close(zip_file) == -1) {
            printw("\nErreur lors de la fermeture du fichier zip.\n");
        }
        else {
            printw("\nLe fichier zip a été correctement mis à jour.\n");
        }
        endwin();
  
        return 0;


      default:
        printw("Invalid choice\n");
        refresh();
        getch();
        break;
      }
    }
  }
  return 0;
}
