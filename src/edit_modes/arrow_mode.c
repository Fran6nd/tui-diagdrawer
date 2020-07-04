#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "position_list.h"
#include "ui.h"
#include "undo_redo.h"

position_list PATH;

static void on_key_event(int c) {
  if (c == K_HELP) {
    ui_show_text_info("You are in the ARROW mode.\n"
                 "You can use [space] to select the starting point\n"
                 "and then [space] again to select the ending point\n"
                 "and draw the arrow!\n"
                 "\n"
                 "Press any key to continue.");
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

static void on_free() { pl_empty(&PATH); }

static character on_draw(position p, character c) {
  int i = pl_is_inside(&PATH, p);
  if (i != -1) {
    c.c = pl_get_arrow_char(&PATH, i);
  }
  return c;
}

static void on_left_column_add(){
  int i = 0;
  for(i = 0; i < PATH.size; i++){
    PATH.list[i].x++;
  }
}
static void on_top_line_add(){
  int i = 0;
  for(i = 0; i < PATH.size; i++){
    PATH.list[i].y++;
  }
}

edit_mode arrow_mode() {
  edit_mode EDIT_MODE_RECT = {.name = "ARROW",
                              .key = (int)'a',
                              .on_key_event = on_key_event,
                              .null = 0,
                              .on_free = on_free,
                              .on_draw = on_draw,
                              .on_left_column_add = on_left_column_add,
                              .on_top_line_add = on_top_line_add};
  return EDIT_MODE_RECT;
}