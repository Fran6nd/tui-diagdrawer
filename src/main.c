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

position P1, P2;
chunk CLIPBOARD;
position_list PATH;
int MODE = MODE_PUT;
int PREVIOUS_MODE = 0;
char *NAME = "Untitled.txt";
position UP_LEFT_CORNER = {0, 0};
chunk CURRENT_FILE;
edit_mode *EDIT_MODE;

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

int is_in_rect(position r1, position r2, position p) {
  position min = pos_min(r1, r2);
  position max = pos_max(r1, r2);
  if (p.x >= min.x && p.x <= max.x)
    if (p.y <= max.y && p.y >= min.y)
      return 1;
  return 0;
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
      char c = chk_get_char_at(&CURRENT_FILE, p);
      if (c != 0) {
        if (EDIT_MODE) {
          if (EDIT_MODE->on_draw)
            c = EDIT_MODE->on_draw(p, c);

        } else if (MODE == MODE_ARROW) {
          int i = pl_is_inside(&PATH, p);
          if (i != -1) {
            c = pl_get_arrow_char(&PATH, i);
          }
        }
        if (pos_on_screen.x == COLS / 2 && pos_on_screen.y == (LINES + 2) / 2) {
          draw_char(pos_on_screen, c, COL_CURSOR);
        } else {
          position tmp = P2.null == 0 ? P2 : get_cursor_pos();
          if (MODE == MODE_SELECT && P1.null == 0 && is_in_rect(P1, tmp, p)) {
            draw_char(pos_on_screen, c, COL_SELECTION);
          } else {
            draw_char(pos_on_screen, c, COL_NORMAL);
          }
        }
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
  addstr("ASCII-DiagDrawer by Fran6nd (press [tab] to swith mode)");
}

position move_cursor(int c) {
  const char *key_name = keyname(c);
  position delta = UP_LEFT_CORNER;
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
    UP_LEFT_CORNER.y -= 5;
  } else if (strcmp(key_name, "kDN5") == 0) {
    UP_LEFT_CORNER.y += 5;
  } else if (strcmp(key_name, "kRIT5") == 0) {
    UP_LEFT_CORNER.x += 5;
  } else if (strcmp(key_name, "kLFT5") == 0) {
    UP_LEFT_CORNER.x -= 5;
  } else {
    delta.null = 1;
    return delta;
  }
  delta.null = 0;
  delta.x = UP_LEFT_CORNER.x - delta.x;
  delta.y = UP_LEFT_CORNER.y - delta.y;
  return delta;
}

int main(int argc, char *argv[]) {
  initscr();
  curs_set(0);
  noecho();
  start_color();
  keypad(stdscr, TRUE);
  register_modes();
  CLIPBOARD.null = 1;

  init_pair(COL_CURSOR, COLOR_WHITE, COLOR_RED);
  init_pair(COL_NORMAL, COLOR_WHITE, COLOR_BLACK);
  init_pair(COL_EMPTY, COLOR_BLACK, COLOR_BLUE);
  init_pair(COL_SELECTION, COLOR_BLACK, COLOR_CYAN);

  P1.null = 1;
  P2.null = 1;
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
      UP_LEFT_CORNER.x++;
      P1.x++;
      P2.x++;
    }
    while (get_cursor_pos().y < 0) {
      chk_add_line_up(&CURRENT_FILE);
      UP_LEFT_CORNER.y++;
      P1.y++;
      P2.y++;
    }
    while (get_cursor_pos().y >= CURRENT_FILE.lines) {
      chk_add_line_down(&CURRENT_FILE);
    }
    while (get_cursor_pos().x >= CURRENT_FILE.cols) {
      chk_add_col_right(&CURRENT_FILE);
    }
    if (!P1.null) {
      while (P1.x < 0) {
        chk_add_col_left(&CURRENT_FILE);
        UP_LEFT_CORNER.x++;
        P1.x++;
        P2.x++;
      }
      while (P1.y < 0) {
        chk_add_line_up(&CURRENT_FILE);
        UP_LEFT_CORNER.y++;
        P1.y++;
        P2.y++;
      }
      while (P1.y >= CURRENT_FILE.lines) {
        chk_add_line_down(&CURRENT_FILE);
      }
      while (P1.x >= CURRENT_FILE.cols) {
        chk_add_col_right(&CURRENT_FILE);
      }
    }
    if (!P2.null) {
      while (P2.x < 0) {
        chk_add_col_left(&CURRENT_FILE);
        UP_LEFT_CORNER.x++;
        P1.x++;
        P2.x++;
      }
      while (P2.y < 0) {
        chk_add_col_left(&CURRENT_FILE);
        UP_LEFT_CORNER.y++;
        P1.y++;
        P2.y++;
      }
      while (P2.y >= CURRENT_FILE.lines) {
        chk_add_line_down(&CURRENT_FILE);
      }
      while (P2.x >= CURRENT_FILE.cols) {
        chk_add_col_right(&CURRENT_FILE);
      }
    }
    draw();
    int c = getch();
    {
      /* [tab] = MENU */
      if (c == '\t') {
        ui_show_text("Press [q] to exit\n"
                     "      [p] to enter PUT mode\n"
                     "      [t] to enter TEXT mode\n"
                     "      [s] to enter SELECT mode\n"
                     "      [r] to enter RECT mode\n"
                     "      [l] to enter LINE mode\n"
                     "      [a] to enter ARROW mode\n"
                     "      [w] to write to file\n"
                     "      [x] to write to file and exit\n"
                     "      [Ctrl] + [r] to redo changes\n"
                     "      [Ctrl] + [u] to undo changes\n"
                     "      [Ctrl] + [h] to show help for the current mode");
        PREVIOUS_MODE = MODE;

        P1.null = 1;
        pl_empty(&PATH);
        EDIT_MODE = NULL;
        c = getch();
        EDIT_MODE = get_edit_mode(c);
        if (EDIT_MODE != NULL) {
        } else {

          switch ((char)c) {
          case 's':
            MODE = MODE_SELECT;
            break;
          case 'p':
            MODE = MODE_PUT;
            break;
          case 'q':
            looping = 0;
            break;
          case 't':
            MODE = MODE_TEXT;
            break;
          case 'a':
            MODE = MODE_ARROW;
            break;
          case 'w':
            chk_save_to_file(&CURRENT_FILE, NAME);
            MODE = PREVIOUS_MODE;
            break;
          case 'x':
            chk_save_to_file(&CURRENT_FILE, NAME);
            looping = 0;
            break;
          case 'l':
            MODE = MODE_LINE;
            break;
          case '\t':
            MODE = PREVIOUS_MODE;
            break;
          case 'r':
            MODE = MODE_RECT;
            break;
          case '\033':
            getch();
            getch();
            break;
          default:
            break;
          }
        }
        P1.null = 1;
        P2.null = 1;
        pl_empty(&PATH);
      }
      /*[Ctrl] + [u] = UNDO */
      else if (c == K_UNDO) {
        /* If we are doing something, we abort it. */
        if ((!P1.null) || (!P2.null) || (PATH.size != 0)) {
          P1.null = 1;
          P2.null = 1;
          pl_empty(&PATH);
        }
        /* Else we undo the last change. */
        else {
          undo_change(&CURRENT_FILE);
        }
      }
      /* [ctrl] + r = REDO */
      else if (c == K_REDO) {
        P1.null = 1;
        P2.null = 1;
        redo_change(&CURRENT_FILE);
        pl_empty(&PATH);
      }

      else {
        /* If no major key pressed, we now do things depending on selected mod.
         */
        if (EDIT_MODE != NULL) {
          if (EDIT_MODE->on_key_event != NULL)
            EDIT_MODE->on_key_event(c);
        } else if (MODE == MODE_TEXT) {
          if (move_cursor(c).null) {
            if (c == K_HELP) {
              ui_show_text("You are in the TEXT mode.\n"
                           "Just enter any text here.\n"
                           "\n"
                           "Press any key to continue.");
              getch();
            }
            /* Erasing. */
            else if (c == 127 || c == KEY_BACKSPACE) {
              position tmp = get_cursor_pos();
              position position_of_char_to_remove = get_cursor_pos();
              position_of_char_to_remove.x--;
              char chr_to_replace =
                  chk_get_char_at(&CURRENT_FILE, position_of_char_to_remove);
              if (chr_to_replace != '+' && chr_to_replace != '|' &&
                  chr_to_replace != '^' && chr_to_replace != 'v' &&
                  chr_to_replace != '>') {
                int x;
                char buffer[CURRENT_FILE.cols];
                int buffer_index = 0;
                for (x = tmp.x; x < CURRENT_FILE.cols; x++) {
                  position tmp1 = {x, tmp.y};
                  char ch = chk_get_char_at(&CURRENT_FILE, tmp1);
                  if (ch != '+' && ch != '|' && ch != '^' && ch != 'v' &&
                      ch != '>') {
                    buffer[buffer_index] = ch;
                    buffer_index++;
                  } else {
                    break;
                  }
                }
                int i;
                for (i = buffer_index - 1; i >= 0; i--) {
                  position tmp1 = {tmp.x + i - 1, tmp.y};
                  chk_set_char_at(&CURRENT_FILE, tmp1, buffer[i]);
                }
                position tmp1 = {tmp.x + buffer_index - 1, tmp.y};
                chk_set_char_at(&CURRENT_FILE, tmp1, ' ');
                UP_LEFT_CORNER.x--;
              }
            }
            /* Deleting. */
            else if (c == KEY_DC) {
              position tmp = get_cursor_pos();
              position position_of_char_to_remove = get_cursor_pos();
              char chr_to_replace =
                  chk_get_char_at(&CURRENT_FILE, position_of_char_to_remove);
              if (!is_diag_char(chr_to_replace)) {
                int x;
                char buffer[CURRENT_FILE.cols];
                int buffer_index = 0;
                for (x = tmp.x; x < CURRENT_FILE.cols; x++) {
                  position tmp1 = {x, tmp.y};
                  char ch = chk_get_char_at(&CURRENT_FILE, tmp1);
                  if (!is_diag_char(ch)) {
                    buffer[buffer_index] = ch;
                    buffer_index++;
                  } else {
                    break;
                  }
                }
                int i;
                for (i = buffer_index - 1; i > 0; i--) {
                  position tmp1 = {tmp.x + i - 1, tmp.y};
                  chk_set_char_at(&CURRENT_FILE, tmp1, buffer[i]);
                }
                position tmp1 = {tmp.x + buffer_index - 1, tmp.y};
                chk_set_char_at(&CURRENT_FILE, tmp1, ' ');
              }
            }
            /* New line. */
            else if (c == K_ENTER) {
              /* Let's find a [tab] or more than 2 [spaces] to find the
               * indentation. */
              char current_key = -1;
              char previous_key =
                  chk_get_char_at(&CURRENT_FILE, get_cursor_pos());
              int starting_x = UP_LEFT_CORNER.x;
              for (; get_cursor_pos().x >= -1; UP_LEFT_CORNER.x--) {
                previous_key = current_key;
                current_key = chk_get_char_at(&CURRENT_FILE, get_cursor_pos());
                if (previous_key == ' ' && current_key == ' ') {

                  if (UP_LEFT_CORNER.x != starting_x - 1) {
                    /* We compensate the 2 spaces found. */
                    UP_LEFT_CORNER.x += 2;
                    do_change(&CURRENT_FILE);
                  } else
                    /* The line is empty. */
                    UP_LEFT_CORNER.x++;
                  break;
                } else if ((is_diag_char(current_key)) &&
                           UP_LEFT_CORNER.x != starting_x) {
                  /* We compensate the character found. */
                  UP_LEFT_CORNER.x += 1;
                  do_change(&CURRENT_FILE);
                  break;
                }
              }
              UP_LEFT_CORNER.y++;
              /* Now if we are on a rect border or en arrow we skip it. */
              int loop = 1;
              while (loop) {
                current_key = chk_get_char_at(&CURRENT_FILE, get_cursor_pos());
                if (is_diag_char(current_key)) {
                  UP_LEFT_CORNER.x++;
                } else {
                  loop = 0;
                }
              }

            } else if (is_writable(c)) {
              position tmp = get_cursor_pos();
              char chr_to_replace = chk_get_char_at(&CURRENT_FILE, tmp);
              if (!is_diag_char(chr_to_replace)) {
                chk_set_char_at(&CURRENT_FILE, tmp, c);
                UP_LEFT_CORNER.x++;
              }
            }
          }

        } else if (MODE == MODE_SELECT) {
          if (P1.null || P2.null) {
            if (c == K_HELP) {
              ui_show_text("You are in the SELECT mode.\n"
                           "You have not two points selected.\n"
                           "Use [space] to select another one.\n"
                           "\n"
                           "Press any key to continue.");
              getch();
            } else if (c == 'p') {
              do_change(&CURRENT_FILE);
              chk_blit_chunk(&CURRENT_FILE, &CLIPBOARD, get_cursor_pos());
            }
            if (move_cursor(c).null) {
              if (c == ' ') {
                position tmp = get_cursor_pos();
                if (P1.null) {
                  P1 = tmp;
                } else if (P2.null) {
                  P2 = tmp;
                } else {
                  P1.null = 1;
                  P2.null = 1;
                }
              }
            }
          } else if (!P2.null) {
            position min = pos_min(P1, P2);
            position max = pos_max(P1, P2);
            int x;
            int y;
            if (c == K_HELP) {
              ui_show_text(get_menu());
              getch();
            } else if (c == ' ') {
              P2.null = 1;
              P1.null = 1;
            } else if (c == 'y') {
              chk_free(&CLIPBOARD);
              CLIPBOARD = chk_extract_chunk(&CURRENT_FILE, min, max);
              P2.null = 1;
              P1.null = 1;
            } else if (c == 'd') {
              chk_fill_chunk(&CURRENT_FILE, min, max, ' ');
            } else if (c == 'c') {
              chk_free(&CLIPBOARD);
              CLIPBOARD = chk_extract_chunk(&CURRENT_FILE, min, max);
              P2.null = 1;
              P1.null = 1;
              chk_fill_chunk(&CURRENT_FILE, min, max, ' ');
            } else if (c == KEY_DC) {
              do_change(&CURRENT_FILE);
              position min = pos_min(P1, P2);
              position max = pos_max(P1, P2);
              int x;
              int y;
              for (x = min.x; x <= max.x; x++) {
                for (y = min.y; y <= max.y; y++) {
                  position tmp = {x, y};
                  chk_set_char_at(&CURRENT_FILE, tmp, ' ');
                }
              }
            }
            /*
            Here we enter into the fill mode.
            Basically we replace all characters in the selection by the
            specified one.
            */
            else if (c == 'f') {
              do {
                c = getch();

              } while (!is_writable(c));
              position min = pos_min(P1, P2);
              position max = pos_max(P1, P2);
              int x;
              int y;
              do_change(&CURRENT_FILE);
              for (x = min.x; x <= max.x; x++) {
                for (y = min.y; y <= max.y; y++) {
                  position tmp = {x, y};
                  chk_set_char_at(&CURRENT_FILE, tmp, c);
                }
              }
            }
            /*
            Here we enter into the replace mode.
            Basically we replace all characters in the selection that are not
            spaces by the specified one.
            */
            else if (c == 'r') {
              do {
                c = getch();
              } while (!is_writable(c));
              position min = pos_min(P1, P2);
              position max = pos_max(P1, P2);
              int x;
              int y;
              do_change(&CURRENT_FILE);
              for (x = min.x; x <= max.x; x++) {
                for (y = min.y; y <= max.y; y++) {
                  position tmp = {x, y};
                  if (chk_get_char_at(&CURRENT_FILE, tmp) != ' ')
                    chk_set_char_at(&CURRENT_FILE, tmp, c);
                }
              }
            } else {
              position delta = move_cursor(c);
              if (!delta.null) {
                P1.y += delta.y;
                P1.x += delta.x;
                P2.y += delta.y;
                P2.x += delta.x;
              }
            }
          }
        
        } else if (MODE == MODE_ARROW) {
          if (c == K_HELP) {
            ui_show_text("You are in the ARROW mode.\n"
                         "You can use [space] to select the starting point\n"
                         "and then [space] again to select the ending point\n"
                         "and draw the arrow!\n"
                         "\n"
                         "Press any key to continue.");
            getch();
          } else if (!move_cursor(c).null) {
            if (PATH.size != 0) {
              position tmp = get_cursor_pos();
              while (pl_is_inside(&PATH, tmp) != -1) {
                pl_remove_last(&PATH);
              }
              pl_add(&PATH, tmp);
            }
          } else if (c == ' ') {
            if (PATH.size != 0) {
              int i;
              do_change(&CURRENT_FILE);
              for (i = 0; i < PATH.size; i++) {

                chk_set_char_at(&CURRENT_FILE, PATH.list[i],
                                pl_get_arrow_char(&PATH, i));
              }
              pl_empty(&PATH);
              PATH = pl_new();
            } else {
              pl_add(&PATH, get_cursor_pos());
            }
          }
        }
      }
    }
  }
  endwin();
  chk_free(&CURRENT_FILE);
  free_undo_redo();
  pl_empty(&PATH);
  edit_mode_free();
  chk_free(&CLIPBOARD);
  return 0;
}