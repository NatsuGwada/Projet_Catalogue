#include <stdio.h>
#include <zip.h>

// we define the name of the directory where the files will be extracted to
#define EXTRACT_DIR "./extracted_files"

// returns the number of the files in the zip
int get_number_files(struct zip *zip_file) {
  int num_files = zip_get_num_entries(zip_file, 0);
  if (num_files < 0) {
    printf("Error getting number of files in zip file\n");
    return 1;
  }
  return num_files;
}

// prints the names of the files in the zip
void get_file_names(struct zip *zip_file, const char *filename, int num_files) {
  printf("Files in %s:\n", filename);
  for (int i = 0; i < num_files; ++i) {
    const char *name = zip_get_name(zip_file, i, 0);
    if (!name) {
      printf("Error getting name of file %d\n", i);
    }
    printf("%d: %s\n", i + 1, name);
  }
  printf("\n");
}

// prints the file names and asks the user wich one to print out
void get_file_content(struct zip *zip_file, const char *filename) {
  int num_files = get_number_files(zip_file);
  // Print the names of the files in the zip
  get_file_names(zip_file, filename, num_files);
  // Ask the user which file to print
  printf("Enter the number of the file you want to print: ");
  int file_index;
  scanf("%d", &file_index);
  // Check if the file number is valid and ask again if not
  while (file_index <= 0 || file_index > num_files) {
    printf("Invalid file number\n");
    printf("Enter the number of the file you want to print: ");
    scanf("%d", &file_index);
  }
  // Open the specified file
  file_index = file_index - 1;
  struct zip_file *file = zip_fopen_index(zip_file, file_index, 0);
  if (!file) {
    printf("Error opening file %d\n", file_index);
    return;
  }
  // Get the size of the file
  struct zip_stat file_stat;
  zip_stat_index(zip_file, file_index, 0, &file_stat);
  size_t file_size = file_stat.size;
  if (file_size == 0) {
    printf("File %d is empty\n", file_index + 1);
  } else {
    // Print the content of the file
    printf("Content of file %d:\n", file_index + 1);
    char buffer[1024];
    int len;
    while ((len = zip_fread(file, buffer, sizeof(buffer))) > 0) {
      fwrite(buffer, 1, len, stdout);
    }
    printf("\n");
  }
  zip_fclose(file);
}

// extracting a file from the zip
void extract_file(struct zip *zip_file, const char *filename) {
  int num_files = get_number_files(zip_file);
  // Print the names of the files in the zip
  get_file_names(zip_file, filename, num_files);
  // Ask the user which file to print
  printf("Enter the number of the file you want to extract: ");
  int file_index;
  scanf("%d", &file_index);
  // Check if the file number is valid and ask again if not
  while (file_index <= 0 || file_index > num_files) {
    printf("Invalid file number\n");
    printf("Enter the number of the file you want to print: ");
    scanf("%d", &file_index);
  }
  // Open the specified file
  file_index = file_index - 1;
  struct zip_file *file = zip_fopen_index(zip_file, file_index, 0);
  if (!file) {
    printf("Error opening file %d\n", file_index);
    return;
  }

  // Get the name of the file that will be extracted
  const char *name = zip_get_name(zip_file, file_index, 0);
  if (!name) {
    printf("Error getting name of file %d\n", file_index);
    return;
  }
  // prepare the path of the file that will be extracted
  char path[256];
  snprintf(path, sizeof(path), "%s/%s", EXTRACT_DIR, name);

  // create a new file with the name of the file that will be extracted
  FILE *new_file = fopen(path, "wb");
  if (!new_file) {
    printf("Error creating file %s\n", name);
    return;
  }
  // add the content of the file to the new file
  char buffer[1024];
  int len;
  while ((len = zip_fread(file, buffer, sizeof(buffer))) > 0) {
    fwrite(buffer, 1, len, new_file);
  }
  printf("File %s was extracted successfully to %s\n", name, EXTRACT_DIR);
  // close both files
  fclose(new_file);
  zip_fclose(file);
  printf("\n");
}

int main() {
  // Open the zip file
  const char *filename = "./test.zip";
  struct zip *zip_file = zip_open(filename, 0, NULL);
  if (!zip_file) {
    printf("Error opening zip file %s\n", filename);
    return 1;
  }
  // Print the content of the zip file
  // print_file_content(zip_file, filename);
  int quit = 0;
  while (!quit) {
    printf("Enter 1 to print the content of a file\n");
    printf("Enter 2 to extract a file\n");
    printf("Enter 3 to quit\n");
    int option;
    scanf("%d", &option);
    switch (option) {
    case 1:
      get_file_content(zip_file, filename);
      break;
    case 2:
      extract_file(zip_file, filename);
      break;
    case 3:
      quit = 1;
      break;
    default:
      printf("Invalid option\n");
    }
  }
  // Close the zip file
  zip_close(zip_file);
  return 0;
}
