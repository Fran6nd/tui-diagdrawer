#include "edit_mode.h"

static void on_key_event(edit_mode *em, int c) {}

static char *get_help(edit_mode *em) {
  return "You are in the ARROW mode.\n"
         "You can use [space] to select the starting point\n"
         "and then [space] again to select the ending point\n"
         "and draw the arrow!\n"
         "\n"
         "Press any key to continue.";
}

static void on_free(edit_mode *em) {}

static character on_draw(edit_mode *em, position p, character c) {
  return c;
}

static void on_left_column_add(edit_mode *em) {
}
static void on_top_line_add(edit_mode *em) {
}

static int on_abort(edit_mode *em) {
  return 0;
}

edit_mode plugin_mode(char *path) {
  edit_mode EDIT_MODE_RECT = {.name = "ARROW",
                              .key = (int)'a',
                              .on_key_event = on_key_event,
                              .on_free = on_free,
                              .on_draw = on_draw,
                              .on_left_column_add = on_left_column_add,
                              .on_top_line_add = on_top_line_add,
                              .on_abort = on_abort,
                              .get_help = get_help};
  return EDIT_MODE_RECT;
}