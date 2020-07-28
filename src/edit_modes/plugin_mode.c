#include "edit_mode.h"
#include "ui.h"
#include "undo_redo.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>

typedef struct data data;
struct data {
  lua_State *L;
  int error;
};

data *current_mode_data;



static int check_type(lua_State *L, int index, int type) {
  if (lua_type(L, index) != type) {
    const char *type_found = lua_typename(L, lua_type(L, index));
    const char *type_expected = lua_typename(L, type);
    char buf[200] = {0};
    lua_Debug ar;
    if (lua_getstack(L, 1, &ar)) {
      lua_getinfo(L, "nSl", &ar);
      sprintf(buf, "Error line %d in function %s: %s expected as #%d, got %s",
              ar.currentline, "dff", type_found, index, type_expected);
      ui_show_text_info(buf);
    }
    return 0;
  }
  return 1;
}

static int l_set_char_at(lua_State *L) {
  /* Keep only two args. */
  lua_settop(L, 2);
  /* Let's check our args types. */
  check_type(L, 1, LUA_TTABLE);
  check_type(L, 2, LUA_INT_TYPE);
  /* Now we read table.x and table.y */
  position p;

  lua_getfield(L, 1, "x");
  lua_getfield(L, 1, "y");
  check_type(L, -1, LUA_INT_TYPE);
  check_type(L, -2, LUA_INT_TYPE);
  if (!lua_isinteger(L, -1) || !lua_isinteger(L, -2) || !lua_isinteger(L, 2)) {
    current_mode_data->error = 1;
    lua_pop(L, 3);
    return 0;
  }

  p.x = lua_tointeger(L, -2);
  p.y = lua_tointeger(L, -1);
  /* Now we read the int value ofthe char to draw. */
  int c = lua_tointeger(L, 2);
  /* Now let's pop p.x, p.y and the character from the stack. */
  lua_pop(L, 3);
  /* Now let's call the C func. */
  chk_set_char_at(&CURRENT_FILE, p, c);
  /* number of results */
  return 0;
}

static int l_get_char_at(lua_State *L) {
  lua_settop(L, 1);
  check_type(L, 1, LUA_TTABLE);
  position p;
  lua_getfield(L, 1, "x");
  check_type(L, -1, LUA_INT_TYPE);
  p.x = lua_tointeger(L, -1);
  lua_getfield(L, 1, "y");
  check_type(L, -1, LUA_INT_TYPE);
  p.y = lua_tointeger(L, -1);
  /* Let's pop .x and .y from the stack. */
  lua_pop(L, 2);
  /* Let's push on the stack the returned value. */
  lua_pushnumber(L, chk_get_char_at(&CURRENT_FILE, p));
  return 1;
}

static int l_do_change(lua_State *L) {
  do_change(&CURRENT_FILE);
  return 0;
}

static int l_show_message(lua_State *L) {
  lua_tostring(L, 1);
  check_type(L, 1, LUA_TSTRING);
  ui_show_text((char *)lua_tostring(L, 1));
  return 0;
}

static int l_show_message_blocking(lua_State *L) {
  lua_tostring(L, 1);
  check_type(L, 1, LUA_TSTRING);
  ui_show_text_info((char *)lua_tostring(L, 1));
  return 0;
}

static int l_move_cursor(lua_State *L) {
  check_type(L, 1, LUA_INT_TYPE);
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

static int l_get_cursor_pos(lua_State *L) {
  lua_newtable(L);
  lua_pushinteger(L, get_cursor_pos().x);
  lua_setfield(L, -2, "x");
  lua_pushinteger(L, get_cursor_pos().y);
  lua_setfield(L, -2, "y");
  return 1;
}

static int l_set_cursor_pos(lua_State *L) {
  position p;
  p.null = 0;
  lua_settop(L, 1);
  check_type(L, 1, LUA_TTABLE);
  lua_getfield(L, -1, "x");
  check_type(L, 1, LUA_INT_TYPE);
  lua_getfield(L, 1, "y");
  check_type(L, 1, LUA_INT_TYPE);
  p.x = lua_tointeger(L, -2);
  p.y = lua_tointeger(L, -1);
  position delta = {.x = get_cursor_pos().x - UP_LEFT_CORNER.x,
                    .y = get_cursor_pos().y - UP_LEFT_CORNER.y};
  p.x -= delta.x;
  p.y -= delta.y;
  UP_LEFT_CORNER.x = p.x;
  UP_LEFT_CORNER.y = p.y;
  lua_pop(L, 2);
  return 0;
}

static void bind(lua_State *L) {
  /* Let's push all tools */
  lua_pushcfunction(L, l_do_change);
  lua_setglobal(L, "do_change");
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

static int error_handler(lua_State *L) { return 0; }

int lua_mypcall(lua_State *L, int nargs, int nret) {
  /* calculate stack position for message handler */
  int hpos = lua_gettop(L) - nargs;
  int ret = 0;
  /* push custom error message handler */
  lua_pushcfunction(L, error_handler);
  /* move it before function and arguments */
  lua_insert(L, hpos);
  /* call lua_pcall function with custom handler */
  ret = lua_pcall(L, nargs, nret, hpos);
  /* remove custom error message handler from stack */
  lua_remove(L, hpos);
  /* pass return value of lua_pcall */
  return ret;
}

static void on_key_event(edit_mode *em, int c) {
  data *d = (data *)em->data;
  if (d->error) {
    return;
  }
  lua_State *L = (lua_State *)((data *)em->data)->L;
  lua_getglobal(L, "on_key_event");
  if (lua_isnil(L, 1)) {
    lua_pop(L, 1);
  } else {
    if (lua_isfunction(L, 1)) {
      lua_pushnumber(L, c);
      if (lua_pcall(L, 1, 0, 0)
          /* pops the function and 0 parameters, pushes 0 results */) {
        char buffer[500] = {0};
        sprintf(buffer, "Failed to run on_key_event: %s\n",
                lua_tostring(L, -1));
        ui_show_text_info(buffer);
      }
    }
  }
}

static char *get_help(edit_mode *em) {
  data *d = (data *)em->data;
  if (d->error) {
    return ("Error:\n"
            "An error occured in your LUA script.\n"
            "This edit-mode is now disabled.\n");
  }
  lua_State *L = (lua_State *)((data *)em->data)->L;
  lua_getglobal(L, "get_help");
  if (lua_isnil(L, 1)) {
    lua_pop(L, 1);
  } else {
    if (lua_isfunction(L, 1)) {
      if (lua_mypcall(L, 0, 1)) {
        char buffer[500] = {0};
        sprintf(buffer, "Failed to run get_help: %s\n", lua_tostring(L, -1));
        ui_show_text_info(buffer);
      }
    }
    if (lua_isstring(L, 1)) {
      char *output = (char *)lua_tostring(L, 1);
      lua_pop(L, 1);
      return output;
    }
    lua_pop(L, 1);
  }
  return "No help available.";
}

static void on_free(edit_mode *em) {
  data *d = (data *)em->data;
  lua_close(d->L);
}

static character on_draw(edit_mode *em, position p, character c) {
  data *d = (data *)em->data;
  /* If there is an error, we show everything in RED and disable LUA calls. */
  if (d->error) {
    c.color = COL_CURSOR;
    return c;
  }
  lua_State *L = (lua_State *)d->L;

  lua_getglobal(L, "on_draw");
  if (lua_isnil(L, 1)) {
    lua_pop(L, 1);
  } else {
    if (!lua_isnil(L, 1) && lua_isfunction(L, 1)) {
      lua_newtable(L);
      lua_pushinteger(L, p.x);
      lua_setfield(L, -2, "x");
      lua_pushinteger(L, p.y);
      lua_setfield(L, -2, "y");
      lua_newtable(L);
      lua_pushinteger(L, c.c);
      lua_setfield(L, -2, "character");
      lua_pushinteger(L, c.color);
      lua_setfield(L, -2, "color");
      if (lua_mypcall(L, 2, 1)) {
        char buffer[500] = {0};
        sprintf(buffer, "Failed to run [on_draw]: %s\n", lua_tostring(L, -1));
        ui_show_text_info(buffer);
      }
      if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "character");
        c.c = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_getfield(L, -1, "color");
        c.color = lua_tointeger(L, -1);
        lua_pop(L, 1);
      }
      lua_pop(L, -1);
    }
  }
  return c;
}

static void on_left_column_add(edit_mode *em) {
  lua_State *L = (lua_State *)em->data;
  lua_getglobal(L, "on_left_column_add");
  if (lua_isnil(L, 1)) {
    lua_pop(L, 1);
  } else {
    if (lua_mypcall(L, 0, 0)) {
      char buffer[500] = {0};
      sprintf(buffer, "Failed to run [on_left_column_add]: %s\n",
              lua_tostring(L, -1));
      ui_show_text_info(buffer);
    }
  }
}
static void on_top_line_add(edit_mode *em) {
  lua_State *L = (lua_State *)((data *)em->data)->L;
  lua_getglobal(L, "on_top_line_add");
  if (lua_isnil(L, 1)) {
    lua_pop(L, 1);
  } else {
    if (lua_mypcall(L, 0, 0)) {
      char buffer[500] = {0};
      sprintf(buffer, "Failed to run [on_top_line_add]: %s\n",
              lua_tostring(L, -1));
      ui_show_text_info(buffer);
    }
  }
}

static void on_open(edit_mode *em) { current_mode_data = (data *)em->data; }

static int on_abort(edit_mode *em) { return 0; }

/* Function called from edit_mode.c for any LUA extension:
 * It builds the struct edit_mode, load and bind the script.
 */
edit_mode plugin_mode(char *path) {
  edit_mode EDIT_MODE_PLUGIN = {.on_key_event = on_key_event,
                                .on_free = on_free,
                                .on_draw = on_draw,
                                .on_left_column_add = on_left_column_add,
                                .on_top_line_add = on_top_line_add,
                                .on_abort = on_abort,
                                .get_help = get_help,
                                .on_open = on_open};

  lua_State *L = luaL_newstate();
  data *d = (data *)malloc(sizeof(data));
  d->error = 0;
  d->L = L;
  EDIT_MODE_PLUGIN.data = d;
  int status = luaL_loadfile(L, path);
  if (status) {

    char buffer[500] = {0};
    sprintf(buffer, "Couldn't load file: %s\n", lua_tostring(L, -1));
    ui_show_text_info(buffer);
    EDIT_MODE_PLUGIN.null = 1;
    return EDIT_MODE_PLUGIN;
  } else {
    /* Loading all the std lib, disbling io and os for safety purpose. */
    luaL_openlibs(L);
    lua_pushnil(L);
    lua_setglobal(L, "os");
    lua_pushnil(L);
    lua_setglobal(L, "io");

    /* Ask Lua to run our little script */
    int result = lua_mypcall(L, 0, LUA_MULTRET);
    if (result) {
      char buffer[500] = {0};
      sprintf(buffer, "Failed to run script: %s\n", lua_tostring(L, -1));
      ui_show_text_info(buffer);
      EDIT_MODE_PLUGIN.null = 1;
      return EDIT_MODE_PLUGIN;
    } else {
      bind(L);
      lua_getglobal(L, "NAME");
      if (lua_isnil(L, 1)) {
        goto error;
      } else {
        EDIT_MODE_PLUGIN.name = (char *)lua_tostring(L, 1);
        lua_pop(L, 1);
        lua_getglobal(L, "KEY");
        if (lua_isnil(L, 1)) {
          goto error;
        } else {
          EDIT_MODE_PLUGIN.key = lua_tointeger(L, 1);
          lua_pop(L, 1);
          return EDIT_MODE_PLUGIN;
        }
      }
    }
  error:
    lua_pop(L, 1);
    EDIT_MODE_PLUGIN.null = 1;
    EDIT_MODE_PLUGIN.on_free(&EDIT_MODE_PLUGIN);
    char buf[100] = {0};
    sprintf(buf, "Error:\nCannot load script:\n%s\nMissing NAME or KEY", path);
    ui_show_text_info(buf);
    return EDIT_MODE_PLUGIN;
  }
}