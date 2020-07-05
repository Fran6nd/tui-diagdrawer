#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "ui.h"
#include "undo_redo.h"

static void on_key_event(int c) {
  if (move_cursor(c).null) {
    if (is_writable(c)) {
      position tmp = get_cursor_pos();
      chk_set_char_at(&CURRENT_FILE, tmp, c);
    }
  }
}

static char *get_help() {
  return "You are in the PUT mode.\n"
         "Press a key and it will fill the selected character.\n"
         "\n"
         "Press any key to continue.";
}

edit_mode put_mode() {
  edit_mode EDIT_MODE_PUT = {.name = "PUT",
                             .key = (int)'p',
                             .on_key_event = on_key_event,
                             .get_help = get_help};
  return EDIT_MODE_PUT;
}