#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "ui.h"
#include "undo_redo.h"

position P1, P2;
chunk CLIPBOARD;

int is_in_rect(position r1, position r2, position p) {
  position min = pos_min(r1, r2);
  position max = pos_max(r1, r2);
  if (p.x >= min.x && p.x <= max.x)
    if (p.y <= max.y && p.y >= min.y)
      return 1;
  return 0;
}

static void on_key_event(int c) {
  {
    if (move_cursor(c).null == 1) {
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
          ui_show_text_info(get_menu());
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
    }
  }
}
static void on_exit() {
  P1.null = 1;
  P2.null = 1;
}

static character on_draw(position p, character c) {
  position tmp = P2.null == 0 ? P2 : get_cursor_pos();
  if (P1.null != 1) {
    if (is_in_rect(P1, tmp, p)) {
      c.color = COL_SELECTION;
    }
  }
  return c;
}

static void on_free() { chk_free(&CLIPBOARD); }

static void on_top_line_add() {
  if (!P1.null) {
    P1.y++;
  }
  if (!P2.null) {
    P2.y++;
  }
}
static void on_left_column_add() {
  if (!P1.null) {
    P1.x++;
  }
  if (!P2.null) {
    P2.x++;
  }
}

static int on_abort() {
  if (!P1.null) {
    P1.null = 1;
    P2.null = 1;
    return 1;
  }
  return 0;
}

edit_mode select_mode() {
  CLIPBOARD.null = 1;
  edit_mode EDIT_MODE_RECT = {.name = "SELECT",
                              .key = (int)'s',
                              .on_key_event = on_key_event,
                              .null = 0,
                              .on_exit = on_exit,
                              .on_draw = on_draw,
                              .on_free = on_free,
                              .on_abort = on_abort};
  return EDIT_MODE_RECT;
}