#include <ncurses.h>

// ascii art
void ascii_art() {
  printw("  #####  ######  #     #   #####   #####         #####\n");
  printw("  #      #    #  ##   ##   #   #   #    #        #\n");
  printw("  #      #    #  # # # #   #####   #####  #####  #\n");
  printw("  #      #    #  #  #  #   #       #   #         #\n");
  printw("  #####  ######  #     #   #       #    #        #####\n");
  printw("\n");
}

// testing fucntion
void some_function() {
  printw("This is a test function\n");
  refresh();
  getch();
}

int main() {
  // Initialize ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  int choice = 1;
  char *choices[3] = {"Choice number 1", "Choice  number 2", "Quit"};
  int max_choice = 3;
  int key;
  // Main program loop
  while (1) {
    // Clear the screen
    clear();
    // Print the ascii art
    ascii_art();
    // Display options
    printw("Choose an option:\n");
    for (int i = 0; i < max_choice; ++i) {
      if (choice - 1 == i)
        printw("[ * ]");
      else
        printw("[   ]");
      printw("%s\n", choices[i]);
    }
    refresh();
    // Get user input
    key = getch();
    // Handle arrow key presses
    if (key == KEY_UP) {
      choice = (choice == 1) ? max_choice : choice - 1;
    } else if (key == KEY_DOWN) {
      choice = (choice == max_choice) ? 1 : choice + 1;
    } else if (key == 10) { // Enter key pressed
      // switch case for all the options
      switch (choice) {
      case 1:
        some_function();
        refresh();
        getch();
        break;
      case 2:
        printw("You chose : %s\n", choices[choice - 1]);
        refresh();
        getch();
        break;
      case 3:
        refresh();
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
