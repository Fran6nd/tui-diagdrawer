#include "edit_mode.h"
#include "ui.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static void on_key_event(edit_mode *em, int c) {}

static char *get_help(edit_mode *em) {
  lua_getglobal((lua_State *)em->data, "HELP");
  char *help = (char *)lua_tostring((lua_State *)em->data, 1);
  lua_pop((lua_State *)em->data, 1);
  return help;
}

static void on_free(edit_mode *em) { lua_close((lua_State *)em->data); }

static character on_draw(edit_mode *em, position p, character c) { return c; }

static void on_left_column_add(edit_mode *em) {}
static void on_top_line_add(edit_mode *em) {}

static int on_abort(edit_mode *em) { return 0; }

edit_mode plugin_mode(char *path) {
  edit_mode EDIT_MODE_RECT = {.on_key_event = on_key_event,
                              .on_free = on_free,
                              .on_draw = on_draw,
                              .on_left_column_add = on_left_column_add,
                              .on_top_line_add = on_top_line_add,
                              .on_abort = on_abort,
                              .get_help = get_help};

  lua_State *L = luaL_newstate();
  EDIT_MODE_RECT.data = (void *)L;
  int status = luaL_loadfile(L, path);
  if (status) {

    char buffer[500] = {0};
    sprintf(buffer, "Couldn't load file: %s\n", lua_tostring(L, -1));
    ui_show_text_info(buffer);
    EDIT_MODE_RECT.null = 1;
    return EDIT_MODE_RECT;
  } else { /* Ask Lua to run our little script */
    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result) {
      char buffer[500] = {0};
      sprintf(buffer, "Failed to run script: %s\n", lua_tostring(L, -1));
      ui_show_text_info(buffer);
      EDIT_MODE_RECT.null = 1;
      return EDIT_MODE_RECT;
    } else {
      lua_getglobal(L, "NAME");
      EDIT_MODE_RECT.name = (char *)lua_tostring(L, 1);
      lua_pop(L, 1);
      lua_getglobal(L, "KEY");
      EDIT_MODE_RECT.key = lua_tointeger(L, 1);
      lua_pop(L, 1);
      return EDIT_MODE_RECT;
    }
  }
}