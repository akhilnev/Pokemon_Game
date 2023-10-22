#include <ncurses.h>

int main() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    int ch;
    printw("Press 'q' to quit.\n");

    while ((ch = getch()) != 'q') {
        printw("You pressed: %c\n", ch);
    }

    endwin();
    return 0;
}
