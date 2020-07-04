#include <curses.h>
#include <stdlib.h>

#include "edit_mode.h"
#include "ui.h"

struct edit_mode *modes;
int edit_mode_counter;

/* Here we add an edit_mode after checking if there is no conflicts with the key
 * used. */
void register_mode(edit_mode em) {
  int i;
  for (i = 0; i < edit_mode_counter; i++) {
    if (modes[i].key == em.key) {
      ui_show_text_info(
          "Error\nTrying to register an edit mode\n using an already "
          "used key.");
      return;
    }
  }
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

/* Here we register all edit modes that are gonna be availables. */
int register_modes() {
  register_mode(rect_mode());
  register_mode(put_mode());
  register_mode(line_mode());
  register_mode(arrow_mode());
  register_mode(text_mode());
  register_mode(select_mode());
  return 1;
}

char *get_menu() {
  /* #TODO: build the menu dynamically from registered edit modes. */
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

/* Return if found the edit mode using the key given as parameter. */
edit_mode *get_edit_mode(int key) {
  int i = 0;
  for (; i < edit_mode_counter; i++) {
    if (key == modes[i].key) {
      return modes + i;
    }
  }
  return NULL;
}

/* Called at when the app is closing... */
void edit_mode_free() {
  int i;
  for (i = 0; i < edit_mode_counter; i++) {
    if (modes[i].on_free != NULL) {
      modes[i].on_free();
    }
  }
  free(modes);
}