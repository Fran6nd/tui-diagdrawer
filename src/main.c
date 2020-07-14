#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "position_list.h"
#include "ui.h"
#include "undo_redo.h"

char *NAME = "Untitled.txt";
position UP_LEFT_CORNER = {0, 0};
chunk CURRENT_FILE;
edit_mode *EDIT_MODE;
edit_mode *PREVIOUS_EDIT_MODE;

position get_cursor_pos() {
  position tmp = {UP_LEFT_CORNER.x + COLS / 2,
                  UP_LEFT_CORNER.y + (LINES - 2) / 2};
  return tmp;
}

void clear_screen() {
  int x;
  for (x = 0; x < COLS; x++) {
    int y;
    for (y = 0; y < LINES; y++) {
      move(y, x);
      addch(' ');
    }
  }
}

int is_writable(char c) {
  char *charset =
      "azertyuiopqsdfghjklmwxcvbn?,.;/"
      ":§!\\`_-'+*#=()[]{}^$&1234567890AZERTYUIOPQSDFGHJKLMWXCVBN <>éèç&$@\"";
  int i;
  for (i = 0; i < strlen(charset); i++) {
    if (charset[i] == c)
      return 1;
  }
  return 0;
}

/* To know whether or not if a char is par of a diagram. */
int is_diag_char(char c) {
  if (c == '|' || c == '+' || c == '-' || c == '<' || c == '>' || c == '^' ||
      c == 'v') {
    return 1;
  }
  return 0;
}

void draw_char(position p, char c, int col) {
  attron(COLOR_PAIR(col));
  move(p.y, p.x);
  addch(c);
  attroff(COLOR_PAIR(col));
}

void draw_file() {
  int x;
  for (x = 0; x < COLS; x++) {
    int y;
    for (y = 1; y < LINES; y++) {
      position pos_on_screen = {x, y};
      position p = {x, y - 2};
      p.x += UP_LEFT_CORNER.x;
      p.y += UP_LEFT_CORNER.y;
      character c = {.c = chk_get_char_at(&CURRENT_FILE, p),
                     .color = COL_NORMAL};
      if (c.c != 0) {
        if (EDIT_MODE) {
          if (EDIT_MODE->on_draw)
            c = EDIT_MODE->on_draw(EDIT_MODE, p, c);
        }
        if (pos_on_screen.x == COLS / 2 && pos_on_screen.y == (LINES + 2) / 2) {
          c.color = COL_CURSOR;
        }

        draw_char(pos_on_screen, c.c, c.color);
      } else {
        draw_char(pos_on_screen, ' ', COL_EMPTY);
      }
    }
  }
}

void draw() {
  draw_file();
  position ui_zero = {0, 0};
  position ui_max = {COLS - 1, LINES - 1};
  ui_draw_rect(ui_zero, ui_max);
  refresh();
  move(0, 2);
  addstr("TUI-DiagDrawer by Fran6nd (press [tab] to switch mode)");
}

position move_cursor(int c) {
  const char *key_name = keyname(c);
  position delta = UP_LEFT_CORNER;
  int i;
  if (strcmp(key_name, "KEY_UP") == 0) {
    UP_LEFT_CORNER.y--;
  } else if (strcmp(key_name, "KEY_DOWN") == 0) {
    UP_LEFT_CORNER.y++;
  } else if (strcmp(key_name, "KEY_RIGHT") == 0) {
    UP_LEFT_CORNER.x++;
  } else if (strcmp(key_name, "KEY_LEFT") == 0) {
    UP_LEFT_CORNER.x--;
  }
  /* If ctrl key is pressed: we move faster. */
  else if (strcmp(key_name, "kUP5") == 0) {
    for (i = 1; i < 6; i++) {
      UP_LEFT_CORNER.y--;
      if (i != 1 && chk_get_char_at(&CURRENT_FILE, get_cursor_pos()) != ' ' &&
          i < 6) {
        UP_LEFT_CORNER.y++;
      }
    }
  } else if (strcmp(key_name, "kDN5") == 0) {
    for (i = 1; i < 6; i++) {
      UP_LEFT_CORNER.y++;
      if (i != 1 && chk_get_char_at(&CURRENT_FILE, get_cursor_pos()) != ' ' &&
          i < 6) {
        UP_LEFT_CORNER.y--;
      }
    }
  } else if (strcmp(key_name, "kRIT5") == 0) {
    for (i = 1; i < 6; i++) {
      UP_LEFT_CORNER.x++;
      if (i != 1 && chk_get_char_at(&CURRENT_FILE, get_cursor_pos()) != ' ' &&
          i < 6) {
        UP_LEFT_CORNER.x--;
      }
    }
  } else if (strcmp(key_name, "kLFT5") == 0) {
    for (i = 1; i < 6; i++) {
      UP_LEFT_CORNER.x--;
      if (i != 1 && chk_get_char_at(&CURRENT_FILE, get_cursor_pos()) != ' ' &&
          i < 6) {
        UP_LEFT_CORNER.x++;
      }
    }
  } else {
    delta.null = 1;
    return delta;
  }
  delta.null = 0;
  delta.x = delta.x - UP_LEFT_CORNER.x;
  delta.y = delta.y - UP_LEFT_CORNER.y;
  return delta;
}

int main(int argc, char *argv[]) {
  initscr();
  curs_set(0);
  noecho();
  start_color();
  keypad(stdscr, TRUE);
  register_modes();
  EDIT_MODE = &modes[0];
  PREVIOUS_EDIT_MODE = EDIT_MODE;

  init_pair(COL_CURSOR, COLOR_WHITE, COLOR_RED);
  init_pair(COL_NORMAL, COLOR_WHITE, COLOR_BLACK);
  init_pair(COL_EMPTY, COLOR_BLACK, COLOR_BLUE);
  init_pair(COL_SELECTION, COLOR_BLACK, COLOR_CYAN);

  if (argc != 2)
    CURRENT_FILE = chk_new(COLS - 2, LINES - 2);
  else {
    CURRENT_FILE = chk_new_from_file(argv[1]);
    NAME = argv[1];
  }
  int looping = 1;
  do_change(&CURRENT_FILE);
  while (looping) {
    while (get_cursor_pos().x < 0) {
      chk_add_col_left(&CURRENT_FILE);
      int i;
      for (i = 0; i < edit_mode_counter; i++) {
        if (modes[i].on_left_column_add != NULL) {
          modes[i].on_left_column_add(&modes[i]);
        }
      }
      UP_LEFT_CORNER.x++;
    }
    while (get_cursor_pos().y < 0) {
      int i;
      chk_add_line_up(&CURRENT_FILE);
      for (i = 0; i < edit_mode_counter; i++) {
        if (modes[i].on_top_line_add != NULL) {
          modes[i].on_top_line_add(&modes[i]);
        }
      }
      UP_LEFT_CORNER.y++;
    }
    while (get_cursor_pos().y >= CURRENT_FILE.lines) {
      chk_add_line_down(&CURRENT_FILE);
    }
    while (get_cursor_pos().x >= CURRENT_FILE.cols) {
      chk_add_col_right(&CURRENT_FILE);
    }
    draw();
    int c = getch();
    {
      /* [tab] = MENU */
      if (c == '\t') {
        ui_show_text(get_menu());
        PREVIOUS_EDIT_MODE = EDIT_MODE;
        if (EDIT_MODE->on_abort)
          EDIT_MODE->on_abort(EDIT_MODE);
        EDIT_MODE = NULL;
        do {
          c = getch();

          switch ((char)c) {
          case 'q':
            looping = 0;
            break;
          case 'w':
            chk_save_to_file(&CURRENT_FILE, NAME);
            break;
          case 'x':
            chk_save_to_file(&CURRENT_FILE, NAME);
            looping = 0;
            break;
          case '\t':
            EDIT_MODE = PREVIOUS_EDIT_MODE;
            break;
          case '\033':
            getch();
            getch();
            break;
          default:
            EDIT_MODE = get_edit_mode(c);
            if (EDIT_MODE->on_open) {
              EDIT_MODE->on_open(EDIT_MODE);
            }
            break;
          }
        } while (EDIT_MODE == NULL && looping == 1);
      }
      /*[Ctrl] + [u] = UNDO */
      else if (c == K_UNDO) {
        /* If we are doing something, we abort it. */
        int aborted = 0;
        if (EDIT_MODE->on_abort) {
          aborted = EDIT_MODE->on_abort(EDIT_MODE);
        }
        /* Else we undo the last change. */
        if (!aborted) {
          undo_change(&CURRENT_FILE);
        }
      }
      /* [ctrl] + r = REDO */
      else if (c == K_REDO) {
        if (EDIT_MODE->on_abort) {
          EDIT_MODE->on_abort(EDIT_MODE);
        }
        redo_change(&CURRENT_FILE);
      }
      /* [ctrl] + h = HELP */
      else if (c == K_HELP) {
        if (EDIT_MODE->get_help) {
          ui_show_text_info(EDIT_MODE->get_help(EDIT_MODE));
        }
      }

      else {
        /* If no major key pressed, we now do things depending on selected mod.
         */
        if (EDIT_MODE != NULL) {
          if (EDIT_MODE->on_key_event != NULL)
            EDIT_MODE->on_key_event(EDIT_MODE, c);
        }
      }
    }
  }
  endwin();
  chk_free(&CURRENT_FILE);
  free_undo_redo();
  edit_mode_free();
  return 0;
}