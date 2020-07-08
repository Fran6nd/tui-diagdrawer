#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "position_list.h"
#include "ui.h"
#include "undo_redo.h"

position_list PATH;

static void on_key_event(edit_mode * self, int c) {
  if (!move_cursor(c).null) {
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
                        pl_get_line_char(&PATH, i));
      }
      pl_empty(&PATH);
      PATH = pl_new();
    } else {
      pl_add(&PATH, get_cursor_pos());
    }
  }
}

static char *get_help(edit_mode * self) {
  return "You are in the LINE mode.\n"
         "You can use [space] to select the first point\n"
         "and then [space] again to select the second point\n"
         "and draw the line!\n"
         "\n"
         "Press any key to continue.";
}

static void on_free(edit_mode * self) { pl_empty(&PATH); }

static character on_draw(edit_mode * self, position p, character c) {
  int i = pl_is_inside(&PATH, p);
  if (i != -1) {
    c.c = pl_get_line_char(&PATH, i);
  }
  return c;
}

static void on_left_column_add(edit_mode * self) {

  int i = 0;
  for (i = 0; i < PATH.size; i++) {
    PATH.list[i].x++;
  }
  if (PATH.size != 0)
    pl_add(&PATH, get_cursor_pos());
}
static void on_top_line_add(edit_mode * self) {
  int i = 0;
  for (i = 0; i < PATH.size; i++) {
    PATH.list[i].y++;
  }
  if (PATH.size != 0)
    pl_add(&PATH, get_cursor_pos());
}

static int on_abort(edit_mode * self) {
  if (PATH.size != 0) {
    pl_empty(&PATH);
    return 1;
  }
  return 0;
}

edit_mode line_mode() {
  edit_mode EDIT_MODE_RECT = {.name = "LINE",
                              .key = (int)'l',
                              .on_key_event = on_key_event,
                              .on_free = on_free,
                              .on_draw = on_draw,
                              .on_top_line_add = on_top_line_add,
                              .on_left_column_add = on_left_column_add,
                              .on_abort = on_abort,
                              .get_help = get_help,
                              .null = 0};
  return EDIT_MODE_RECT;
}