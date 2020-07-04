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
    ui_show_text("You are in the LINE mode.\n"
                 "You can use [space] to select the first point\n"
                 "and then [space] again to select the second point\n"
                 "and draw the line!\n"
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
                        pl_get_line_char(&PATH, i));
      }
      pl_empty(&PATH);
      PATH = pl_new();
    } else {
      pl_add(&PATH, get_cursor_pos());
    }
  }
}

static void on_exit() {
  pl_empty(&PATH);
}

static int is_on_rect_corner(position r1, position r2, position p) {
  position down_left = pos_min(r1, r2);
  position up_right = pos_max(r1, r2);
  position up_left = {down_left.x, up_right.y};
  position down_right = {up_right.x, down_left.y};
  if (p.x == down_left.x && p.y == down_left.y)
    return 1;
  if (p.x == up_right.x && p.y == up_right.y)
    return 1;
  if (p.x == up_left.x && p.y == up_left.y)
    return 1;
  if (p.x == down_right.x && p.y == down_right.y)
    return 1;
  return 0;
}
static int is_on_rect_border(position r1, position r2, position p) {
  position min = pos_min(r1, r2);
  position max = pos_max(r1, r2);
  if (p.x > min.x && p.x < max.x)
    if (p.y == max.y || p.y == min.y)
      return 1;
  if (p.y > min.y && p.y < max.y)
    if (p.x == max.x || p.x == min.x)
      return 1;
  return 0;
}

static char on_draw(position p, char c) {
  int i = pl_is_inside(&PATH, p);
  if (i != -1) {
    c = pl_get_line_char(&PATH, i);
  }
  return c;
}

edit_mode line_mode() {
  edit_mode EDIT_MODE_RECT = {.name = "LINE",
                              .key = (int)'l',
                              .on_key_event = on_key_event,
                              .null = 0,
                              .on_exit = on_exit,
                              .on_draw = on_draw};
  return EDIT_MODE_RECT;
}