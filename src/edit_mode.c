#include <curses.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "edit_mode.h"
#include "ui.h"

struct edit_mode *modes;
int edit_mode_counter;
char menu_buffer[10000] = {0};

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
void register_modes() {
  /* Let's register all edit_modes. */
  register_mode(rect_mode());
  register_mode(put_mode());
  register_mode(line_mode());
  register_mode(arrow_mode());
  register_mode(text_mode());
  register_mode(select_mode());
  /* Let's register our Lua plugins. */

  char *dir = "/.tui-diagdrawer";
  char path[100] = {0};
  strcpy(path, getenv("HOME"));
  strcat(path, dir);
  struct stat sb;

  /* If the script folder is not existing, we create it. */
  if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
  } else {
    mkdir(path, 0777);
  }
  /*Let's iterate over LUA files.*/
  DIR *folder;
  struct dirent *entry;
  int files = 0;
  folder = opendir(path);
  if (folder == NULL) {
    ui_show_text_info("Error:\nCannot read ~/.tui_diagdrawer.");
  } else {

    while ((entry = readdir(folder))) {
      files++;
      if (strcmp("..", entry->d_name) != 0 && strcmp(".", entry->d_name) != 0) {
        char filename[100] = {0};
        strcpy(filename, path);
        strcat(filename, "/");
        strcat(filename, entry->d_name);
        if (filename[strlen(filename) - 1] == 'a')
          if (filename[strlen(filename) - 2] == 'u')
            if (filename[strlen(filename) - 3] == 'l')
              if (filename[strlen(filename) - 4] == '.')
                ui_show_text_info(filename);
      }
      // register_mode(plugin_mode(entry->d_name));
    }

    closedir(folder);
  }
  /* Here we build the menu dynamically from registered edit modes. */
  char *common_line = "      [%c] to enter %s mode\n";
  char mode_help[1000] = {0};
  int i;
  for (i = 0; i < edit_mode_counter; i++) {
    char buf[100];
    memset(buf, 0, 100);
    strcpy(buf, common_line);
    sprintf(buf, common_line, modes[i].key, modes[i].name);
    strcat(mode_help, buf);
  }
  char *help = "Press [q] to exit\n"
               "      [w] to write to file\n"
               "      [x] to write to file and exit\n"
               "%s"
               "      [Ctrl] + [r] to redo changes\n"
               "      [Ctrl] + [u] to undo changes\n"
               "      [Ctrl] + [h] to show help for the current mode";
  memset(menu_buffer, 0, 10000);
  sprintf(menu_buffer, help, mode_help);
}

char *get_menu() {
  /* Return the string of the menu built from registered edit_modes. */
  return menu_buffer;
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
      modes[i].on_free(&modes[i]);
    }
  }
  free(modes);
}