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
  char * common_line = "";
  return ("Press [q] to exit\n"
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