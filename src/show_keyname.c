#include <curses.h>
#include <stdio.h>

/*
 * Basic program to display pressed key's name.
 */

int main() {
  initscr();
  noecho();
  keypad(stdscr, 1);
  while (1) {
    const int c = getch();
    const char *name = keyname(c);
    printw("key: %s code: %d |", name, c);
    if (c == 'q')
      break;
  }
  endwin();
}