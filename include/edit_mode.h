#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include "chunk.h"
#include "main.h"

typedef struct edit_mode {
  /* The name of the edit_mode. */
  char *name;
  /* The key used to access the edit_mode. */
  int key;
  /* Function used to manage events. */
  void (*on_key_event)(int);
  /* Function used to hook character drawing. */
  character (*on_draw)(position, character);
  /* Function used to get the help to display on [ctrl] + [h]. */
  char *(*get_help)();
  /* These two functions are called when we move all characters to left or down
   * by adding a line on top or a column on the left side.
   * They are used to tell the edit_mode that everything moved. */
  void (*on_top_line_add)();
  void (*on_left_column_add)();
  /* Called when exiting the current mode. */
  void (*on_exit)();
  /* Called when the app is closing. */
  void (*on_free)();
  /* Used to abort the current task.
   * Return 1 if a task has been aborted, else 0. */
  int (*on_abort)();
} edit_mode;

extern struct edit_mode *modes;
extern int edit_mode_counter;

void register_modes();
char *get_menu();
edit_mode *get_edit_mode(int key);
void edit_mode_free();

/* Functions returning all natives edit modes. */
edit_mode put_mode();
edit_mode rect_mode();
edit_mode arrow_mode();
edit_mode line_mode();
edit_mode text_mode();
edit_mode select_mode();

#endif