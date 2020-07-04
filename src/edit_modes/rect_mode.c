#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "ui.h"
#include "undo_redo.h"

position P1, P2;

static void on_key_event(int c) {

  if (c == K_HELP) {
    ui_show_text_info("You are in the RECT mode.\n"
                      "You can draw any rect by using [space] to select the "
                      "first point\n"
                      "and [space] again to select the second one.\n"
                      "\n"
                      "Press any key to continue.");
  } else if (move_cursor(c).null) {
    if (c == (int)' ') {
      position tmp = get_cursor_pos();
      if (P1.null) {
        P1 = tmp;
      } else {
        do_change(&CURRENT_FILE);
        position down_left = pos_min(P1, tmp);
        position up_right = pos_max(P1, tmp);
        position up_left = {down_left.x, up_right.y};
        position down_right = {up_right.x, down_left.y};
        chk_set_char_at(&CURRENT_FILE, down_left, '+');
        chk_set_char_at(&CURRENT_FILE, up_right, '+');
        chk_set_char_at(&CURRENT_FILE, down_right, '+');
        chk_set_char_at(&CURRENT_FILE, up_left, '+');
        int x;
        for (x = down_left.x + 1; x < down_right.x; x++) {
          position tmp1 = {x, up_right.y};
          chk_set_char_at(&CURRENT_FILE, tmp1, '-');
        }
        for (x = down_left.x + 1; x < down_right.x; x++) {
          position tmp1 = {x, down_right.y};
          chk_set_char_at(&CURRENT_FILE, tmp1, '-');
        }
        int y;
        for (y = down_left.y + 1; y < up_left.y; y++) {
          position tmp1 = {up_left.x, y};
          chk_set_char_at(&CURRENT_FILE, tmp1, '|');
        }
        for (y = down_left.y + 1; y < up_left.y; y++) {
          position tmp1 = {up_right.x, y};
          chk_set_char_at(&CURRENT_FILE, tmp1, '|');
        }

        P1.null = 1;
      }
    }
  }
}

static void on_exit() {
  P1.null = 1;
  P2.null = 1;
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

static character on_draw(position p, character c) {
  if (P1.null == 0) {
    if (is_on_rect_corner(P1, get_cursor_pos(), p)) {
      c.c = '+';
    } else if (is_on_rect_border(P1, get_cursor_pos(), p)) {
      if (p.y == P1.y || p.y == get_cursor_pos().y) {
        c.c = '-';
      }
      if (p.x == P1.x || p.x == get_cursor_pos().x) {
        c.c = '|';
      }
    }
  }
  return c;
}

static void on_top_line_add() {
  if (!P1.null) {
    P1.y++;
  }
}
static void on_left_column_add() {
  if (!P1.null) {
    P1.x++;
  }
}

static int on_abort(){
  if(!P1.null){
    P1.null = 1;
    P2.null = 1;
    return 1;
  }
  return 0;
}

edit_mode rect_mode() {
  P1.null = 1;
  P2.null = 1;
  edit_mode EDIT_MODE_RECT = {.name = "RECT",
                              .key = 114,
                              .on_key_event = on_key_event,
                              .null = 0,
                              .on_exit = on_exit,
                              .on_draw = on_draw,
                              .on_top_line_add = on_top_line_add,
                              .on_left_column_add = on_left_column_add,
                              .on_abort = on_abort};
  return EDIT_MODE_RECT;
}