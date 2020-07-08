#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include "chunk.h"
#include "main.h"

typedef struct edit_mode edit_mode;
struct edit_mode {
  /* The name of the edit_mode. */
  char *name;
  /* The key used to access the edit_mode. */
  int key;
  /* Function used to manage events. */
  void (*on_key_event)(edit_mode *, int);
  /* Function used to hook character drawing. */
  character (*on_draw)(edit_mode *, position, character);
  /* Function used to get the help to display on [ctrl] + [h]. */
  char *(*get_help)(edit_mode *);
  /* These two functions are called when we move all characters to left or down
   * by adding a line on top or a column on the left side.
   * They are used to tell the edit_mode that everything moved. */
  void (*on_top_line_add)(edit_mode *);
  void (*on_left_column_add)(edit_mode *);
  /* Called when exiting the current mode. */
  void (*on_exit)(edit_mode *);
  /* Called when the app is closing. */
  void (*on_free)(edit_mode *);
  /* Used to abort the current task.
   * Return 1 if a task has been aborted, else 0. */
  int (*on_abort)(edit_mode *);
  /* Used to store a custom data into the edit_mode struct sucj as a lua script.
   */
  void *data;
  /* Used to knowif NULL1 */
  int null;s
};

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
edit_mode plugin_mode(char *);

#endif