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


// Structure for list
struct Node
{
  char token[256];
  struct Node *next;
};

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

// Function to add a token to the list (Gordibus)
void add_token_to_list(struct Node **head, const char *token)
{
struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error allocating memory for the new node.\n");
        exit(1);
    }
    strcpy(new_node->token, token);
    new_node->next = NULL; // Make sure the new node points to NULL, as it will be the last node
    if (*head == NULL)
    {
        // If the list is empty, set the new node as the head
        *head = new_node;
    }
    else
    {
        // Find the last node in the list
        struct Node *temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        // Append the new node to the end of the list
        temp->next = new_node;
    }
}


// Function to free the memory occupied by the list  (gordibus)
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






// ajouter un fichier a l'archive ouverte

int ajout_fichier_au_zip(struct zip *zip_file, const char *file_path) {

  //ouvrir le fichier a ajouter
  FILE *fichier_a_ajouter = fopen(file_path, "rb");
  if (!fichier_a_ajouter) {
    printw("Error , File cannot open %s \n", file_path);
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
  } else {
    printw("File %s has added to the zip successfully / y bon y adan ZIP là", file_path);
  }

  

  //fèmé tout

  fclose(fichier_a_ajouter);
  zip_source_free(source);


  return 0;

}

// print the help message
void print_help(const char *program_name) {
  printw("Usage : %s [OPTIONS]\n", program_name);
  printw("Options : \n");
  printw("    -a <ton fichier> add a File to ZIP / ça chabin c'est pour ajouté ton fichier au zip\n");
  printw("    -f <filename> Specify the input file\n");
  printw("    -h Show this help\n");
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
      filename == NULL;
      if (filename == NULL) {
        printw("Awa bay la vwa si dokiman aw épi opsyon -f prèmyé tan ti misye \n");
        exit(EXIT_FAILURE);
      }
      
      if (ajout_fichier_au_zip(zip_file, optarg) != 0) {
        printw(" Cant add file like this !\n\n ");
        printw("Non zanmi dokiman pa ka add kon sa \n");
        exit(EXIT_FAILURE);
      }else{
        printw(" Add file %s on the ZIP , successfuly !\n\n", optarg);
        printw("OOuuuiii fanmi la dokiman %s aw adan zip la \n!", optarg);
        exit(EXIT_SUCCESS);
      }
      
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
  char *choices[6] = {"Print the content of a file", "Extract a file","Mèt dokiman aw ", "Help / Wiki-aw","Quit","Bruteforce"};
  int max_choice = 6;
  // the pressed key
  int key;
  // Open the zip file
  zip_file = zip_open(filename, 0, NULL);
  if (!zip_file) {
    printw("Error opening zip file %s\n", filename);
    endwin();
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
          "Bon tu connais déja sé nou mèm amuse toi bien avec notre programme. \U0001F918 BigUp 971-972 \U0001F918\n\n"
          "Le 6e choix est un brute force qui permet de trouver le password des fichier chiffrée dans un ZIP en fonctionne d'une wordlist\n\n");

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
            printw("\n Le fichier zip a été correctement mis à jour.\n");
        }
        endwin();
  
        return 0;

      case 6:
        clear();
        ascii_art();
        printw("Système de BruteForce pour casser le mot de passe chiffer d'un fichier.txt dans une ZIP a l'aide d'une wordlist\n\n");
        refresh();
        // Limit size to 256 characters
        FILE *wordlist;
        char zip_path[256];
        char wordlist_path[256];
        char token[256];

        printf("\n Enter your ZIP file path: ");
        scanf("%255s", zip_path); // Limit size to 255 characters to prevent buffer overflow

        // Open the ZIP file
        int error = 0;
        zip_t *z = zip_open(zip_path, 0, &error);
        if (z == NULL)
        {
            fprintf(stderr, "Error opening the ZIP file.\n");
            return 1;
        }
        printf("\n\n Enter your wordlist file path: ");
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
            // Check if the password works by trying to open the first entry
            struct zip_stat st;
            if (zip_stat_index(zip_with_token, 0, 0, &st) != 0)
            {
                fprintf(stderr, "Error retrieving information for the first entry in the ZIP file with token \"%s\".\n", current->token);
                zip_close(zip_with_token);
                current = current->next;
                continue;
            }
            zip_file_t *f = zip_fopen_index(zip_with_token, 0, 0);
            if (f == NULL)
            {
                fprintf(stderr, "Error opening the first entry in the ZIP file with token \"%s\".\n", current->token);
                zip_close(zip_with_token);
                current = current->next;
                continue;
            }
            zip_fclose(f);

            // If the password works, mark it as found and extract files
            printf("Decompress successful with token: %s\n", current->token);
            found = 1;

            // Extract files
            for (int i = 0; i < num_entries; i++)
            {
                zip_stat_index(zip_with_token, i, 0, &st);
                zip_file_t *file = zip_fopen_index(zip_with_token, i, 0);
                if (file == NULL)
                {
                    fprintf(stderr, "Error opening file %d in the ZIP file with token \"%s\".\n", i, current->token);
                    continue;
                }

                FILE *outfile = fopen(st.name, "wb");
                if (outfile == NULL)
                {
                    fprintf(stderr, "Error creating output file for file %d in the ZIP file with token \"%s\".\n", i, current->token);
                    zip_fclose(file);
                    continue;
                }

                char buf[1024];
                zip_int64_t nbytes;
                while ((nbytes = zip_fread(file, buf, sizeof(buf))) > 0)
                {
                    fwrite(buf, 1, nbytes, outfile);
                }

                fclose(outfile);
                zip_fclose(file);
            }

            zip_close(zip_with_token);
            break;
        }

        if (!found)
        {
            printf("No valid token found in the wordlist, try with another wordlist.\n");
            return 1;
        }

        return 0;
        break;
      


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
