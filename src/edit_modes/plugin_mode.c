#include "edit_mode.h"
#include "ui.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

static int l_set_char_at(lua_State *L) {
  /* Keep only two firsts args. */
  lua_settop(L, 2);
  /* Let's check our args types. */
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_INT_TYPE);
  /* Now we read table.x and table.y */
  position p;
  lua_getfield(L, 1, "x");
  p.x = lua_tointeger(L, -1);
  lua_getfield(L, 1, "y");
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
  luaL_checktype(L, 1, LUA_TTABLE);
  position p;
  lua_getfield(L, 1, "x");
  if (!lua_isnumber(L, -1))
    goto error;
  p.x = lua_tointeger(L, -1);
  lua_getfield(L, 1, "y");
  if (!lua_isnumber(L, -1))
    goto error;
  p.y = lua_tointeger(L, -1);
  /* Let's pop .x and .y from the stack. */
  lua_pop(L, 2);
  /* Let's push on the stack the returned value. */
  lua_pushnumber(L, chk_get_char_at(&CURRENT_FILE, p));
  return 1;
error:
  lua_pop(L, 1);
  lua_pushnil(L);
  return 1;
}

static int l_show_message(lua_State *L) {
  ui_show_text((char *)lua_tostring(L, 1));
  return 0;
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
  ui_show_text_info((char *)lua_tostring(L, 1));
  return 0;
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
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_getfield(L, -1, "x");
  p.x = lua_tointeger(L, -1);
  lua_getfield(L, 1, "y");
  p.y = lua_tointeger(L, -1);
  lua_pop(L, 2);
  position delta = {.x = get_cursor_pos().x - UP_LEFT_CORNER.x,
                    .y = get_cursor_pos().y - UP_LEFT_CORNER.y};
  p.x -= delta.x;
  p.y -= delta.y;
  UP_LEFT_CORNER.x = p.x;
  UP_LEFT_CORNER.y = p.y;

  return 0;
}

static void bind(lua_State *L) {
  /* Let's push all tools */
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
  lua_State *L = (lua_State *)em->data;
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
  lua_State *L = (lua_State *)em->data;
  lua_getglobal(L, "get_help");
  if (lua_isnil(L, 1)) {
    lua_pop(L, 1);
  } else {
    if (lua_isfunction(L, 1)) {
      if (lua_pcall(L, 0, 1, 0)) {
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

static void on_free(edit_mode *em) { lua_close((lua_State *)em->data); }

static character on_draw(edit_mode *em, position p, character c) {

  lua_State *L = (lua_State *)em->data;
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
      if (lua_pcall(L, 2, 1, 0)) {
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
    if (lua_pcall(L, 0, 0, 0)) {
      char buffer[500] = {0};
      sprintf(buffer, "Failed to run [on_left_column_add]: %s\n",
              lua_tostring(L, -1));
      ui_show_text_info(buffer);
    }
  }
}
static void on_top_line_add(edit_mode *em) {
  lua_State *l = (lua_State *)em->data;
  lua_getglobal(l, "on_top_line_add");
  if (lua_isnil(l, 1)) {
    lua_pop(l, 1);
  } else {
    if (lua_pcall(l, 0, 0, 0)) {
      char buffer[500] = {0};
      sprintf(buffer, "Failed to run [on_top_line_add]: %s\n",
              lua_tostring(l, -1));
      ui_show_text_info(buffer);
    }
  }
}

static int on_abort(edit_mode *em) { return 0; }

edit_mode plugin_mode(char *path) {
  edit_mode EDIT_MODE_PLUGIN = {.on_key_event = on_key_event,
                                .on_free = on_free,
                                .on_draw = on_draw,
                                .on_left_column_add = on_left_column_add,
                                .on_top_line_add = on_top_line_add,
                                .on_abort = on_abort,
                                .get_help = get_help};

  lua_State *L = luaL_newstate();
  EDIT_MODE_PLUGIN.data = (void *)L;
  int status = luaL_loadfile(L, path);
  if (status) {

    char buffer[500] = {0};
    sprintf(buffer, "Couldn't load file: %s\n", lua_tostring(L, -1));
    ui_show_text_info(buffer);
    EDIT_MODE_PLUGIN.null = 1;
    return EDIT_MODE_PLUGIN;
  } else {
    /* #FIXME: Allow only some libs. */
    luaL_openlibs(L);
    /* Ask Lua to run our little script */
    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
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