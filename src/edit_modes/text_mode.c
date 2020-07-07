#include <curses.h>

#include "edit_mode.h"
#include "main.h"
#include "position.h"
#include "ui.h"
#include "undo_redo.h"

static void on_key_event(edit_mode * self, int c) {
  if (move_cursor(c).null) {
    /* Erasing. */
    if (c == 127 || c == KEY_BACKSPACE) {
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
          if (ch != '+' && ch != '|' && ch != '^' && ch != 'v' && ch != '>') {
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
      char previous_key = chk_get_char_at(&CURRENT_FILE, get_cursor_pos());
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
}

static char* get_help(edit_mode * self){
  return "You are in the TEXT mode.\n"
                        "Just enter any text here.\n"
                        "\n"
                        "Press any key to continue.";
}

edit_mode text_mode() {
  edit_mode ed = {.name = "TEXT",
                  .key = (int)'t',
                  .on_key_event = on_key_event,
                  .get_help = get_help};
  return ed;
}