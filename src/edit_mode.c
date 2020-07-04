#include <stdlib.h>

#include "edit_mode.h"

struct edit_mode *modes;
int edit_mode_counter;
void register_mode(edit_mode em) {
  if (edit_mode_counter == 0) {
    modes = malloc(1 * sizeof(edit_mode));
    modes[0] = em;
  } else {
    modes = (edit_mode *)realloc(modes,
                                 sizeof(edit_mode) * (edit_mode_counter + 1));
    modes[edit_mode_counter] = em;
  }
  edit_mode_counter++;
}
int register_modes() {
  register_mode(rect_mode());
  register_mode(put_mode());
  register_mode(line_mode());
  return 1;
}
char *get_menu() {
  return ("You are in the SELECT mode.\n"
          "You have selected one rect. Here is what you can do:\n"
          "      [space] to deselect.\n"
          "      [y] to copy to the clipboard.\n"
          "      [c] to cut to the clipboard.\n"
          "      [del] to delete the selection.\n"
          "      [f] then [x] to fill the selection with x.\n"
          "      [r] then [x] to replace non-spaces in the selection "
          "with x.\n"
          "\n"
          "Press any key to continue.");
}
edit_mode *get_edit_mode(int key) {
  int i = 0;
  for (; i < edit_mode_counter; i++) {
    if (key == modes[i].key) {
      return modes + i;
    }
  }
  return NULL;
}

void edit_mode_free() { free(modes); }