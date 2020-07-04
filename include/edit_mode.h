#ifndef EDIT_MODE_H
#define EDIT_MODE_H

#include "chunk.h"

typedef struct edit_mode {
  void (*on_key_event)(int);
  char (*on_draw)(position, char);
  void (*on_exit)();
  char *name;
  int key;
  char *help;
  int null;
} edit_mode;

extern struct edit_mode *modes;
extern int edit_mode_counter;

int register_modes();
char *get_menu();
edit_mode *get_edit_mode(int key);
void edit_mode_free();

edit_mode put_mode();
edit_mode rect_mode();
edit_mode arrow_mode();
edit_mode line_mode();
edit_mode text_mode();
edit_mode select_mode();

#endif