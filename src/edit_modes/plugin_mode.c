#include "edit_mode.h"
#include "ui.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int l_set_char_at(lua_State *L) {
  position p;
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_INT_TYPE);
  lua_getfield(L, 1, "x");
  p.x = lua_tointeger(L, -1);
  lua_getfield(L, 1, "y");
  p.y = lua_tointeger(L, -1);
  int c = lua_tointeger(L, 2);
  lua_pop(L, 3);
  chk_set_char_at(&CURRENT_FILE, p, c);
  return 0; /* number of results */
}
static int l_get_char_at(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  position p;
  lua_getfield(L, 1, "x");
  p.x = lua_tointeger(L, -1);
  lua_getfield(L, 1, "y");
  p.y = lua_tointeger(L, -1);
  lua_pop(L, 2);
  lua_pushnumber(L, chk_get_char_at(&CURRENT_FILE, p));

  return 1;
}

static int l_show_message(lua_State *L) {
  ui_show_text((char *)lua_tostring(L, 1)); /* get argument */
  return 0;                                 /* number of results */
}

static int l_move_cursor(lua_State *L) {
  position p = move_cursor(lua_tointeger(L, 1));
  if (p.null)
    return 0;
  lua_newtable(L);
  lua_pushinteger(L, p.x);
  lua_setfield(L, -2, "x");
  lua_pushinteger(L, p.y);
  lua_setfield(L, -2, "y");
  return 1;
}

static int l_show_message_blocking(lua_State *L) {
  ui_show_text_info((char *)lua_tostring(L, 1)); /* get argument */
  return 0;                                      /* number of results */
}

static int l_get_cursor_pos(lua_State *l) {
  lua_newtable(l);
  lua_pushinteger(l, get_cursor_pos().x);
  lua_setfield(l, -2, "x");
  lua_pushinteger(l, get_cursor_pos().y);
  lua_setfield(l, -2, "y");
  return 1;
}

static int l_set_cursor_pos(lua_State *l) {
  position p;
  p.null = 0;
  lua_settop(l, 1);
  luaL_checktype(l, 1, LUA_TTABLE);
  lua_getfield(l, -1, "x");
  p.x = lua_tointeger(l, -1);
  lua_getfield(l, 1, "y");
  p.y = lua_tointeger(l, -1);
  lua_pop(l, 2);
  position delta = {.x = get_cursor_pos().x - UP_LEFT_CORNER.x,
                    .y = get_cursor_pos().y - UP_LEFT_CORNER.y};
  p.x -= delta.x;
  p.y -= delta.y;
  UP_LEFT_CORNER.x = p.x;
  UP_LEFT_CORNER.y = p.y;

  return 0;
}

static void bind(lua_State *L) {
  lua_pushcfunction(L, l_set_char_at);
  lua_setglobal(L, "set_char_at");
  lua_pushcfunction(L, l_get_char_at);
  lua_setglobal(L, "get_char_at");
  lua_pushcfunction(L, l_move_cursor);
  lua_setglobal(L, "move_cursor");
  lua_pushcfunction(L, l_show_message);
  lua_setglobal(L, "show_message");
  lua_pushcfunction(L, l_show_message_blocking);
  lua_setglobal(L, "show_message_blocking");
  lua_pushcfunction(L, l_get_cursor_pos);
  lua_setglobal(L, "get_cursor_pos");
  lua_pushcfunction(L, l_set_cursor_pos);
  lua_setglobal(L, "set_cursor_pos");
  /* Let's push all colors. */
  lua_pushinteger(L, COL_SELECTION);
  lua_setglobal(L, "COL_SELECTION");
  lua_pushinteger(L, COL_EMPTY);
  lua_setglobal(L, "COL_EMPTY");
  lua_pushinteger(L, COL_NORMAL);
  lua_setglobal(L, "COL_NORMAL");
}

static void on_key_event(edit_mode *em, int c) {
  lua_State *l = (lua_State *)em->data;
  // lua_pushinteger(l, c);
  lua_getglobal(l, "on_key_event");
  if (lua_isfunction(l, 1)) {
    lua_pushnumber(l, c);
    lua_pcall(l, 1, 0,
              0); /* pops the function and 0 parameters, pushes 0 results */
  }
}

static char *get_help(edit_mode *em) {
  lua_getglobal((lua_State *)em->data, "HELP");
  char *help = (char *)lua_tostring((lua_State *)em->data, 1);
  lua_pop((lua_State *)em->data, 1);
  return help;
}

static void on_free(edit_mode *em) { lua_close((lua_State *)em->data); }

static character on_draw(edit_mode *em, position p, character c) {
  lua_State *l = (lua_State *)em->data;
  lua_getglobal(l, "on_draw");
  if (!lua_isnil(l, 1) && lua_isfunction(l, 1)) {
    lua_newtable(l);
    lua_pushinteger(l, p.x);
    lua_setfield(l, -2, "x");
    lua_pushinteger(l, p.y);
    lua_setfield(l, -2, "y");
    lua_newtable(l);
    lua_pushinteger(l, c.c);
    lua_setfield(l, -2, "character");
    lua_pushinteger(l, c.color);
    lua_setfield(l, -2, "color");
    lua_pcall(l, 2, 1, 0);
    if (lua_istable(l, -1)) {
      lua_getfield(l, -1, "character");
      c.c = lua_tointeger(l, -1);
      lua_pop(l, 1);
      lua_getfield(l, -1, "color");
      c.color = lua_tointeger(l, -1);
      lua_pop(l, 1);
    }
    lua_pop(l, -1);
  }
  return c;
}

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
      bind(L);
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