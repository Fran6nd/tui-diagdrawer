#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "plugin.h"

struct plugin {
  lua_State *plugin;
  char *name;
};

int plugin_load(char *path) {
  if (plugins.loaded == 0) {
    plugins.loaded++;
    plugins.plugins = (lua_State **)malloc(sizeof(lua_State *));
  } else {
    plugins.loaded++;
    plugins.plugins = (lua_State **)realloc(
        plugins.plugins, sizeof(lua_State *) * plugins.loaded);
  }
  int status, result, i;
  double sum;

  plugins.plugins[plugins.loaded-1] = luaL_newstate();

  luaL_openlibs(plugins.plugins[plugins.loaded-1]);

  status = luaL_loadfile(plugins.plugins[plugins.loaded-1], path);
  if (status) {
    fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(plugins.plugins[plugins.loaded-1], -1));
    exit(1);
  }

  /* Ask Lua to run our little script */
  result = lua_pcall(plugins.plugins[plugins.loaded-1], 0, LUA_MULTRET, 0);
  if (result) {
    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(plugins.plugins[plugins.loaded-1], -1));
    exit(1);
  }
}
void plugins_load() {
  plugins.loaded = 0;
  plugin_load("lua/custom_mode.lua");
}
void plugins_free() {
  int i;
  if (plugins.loaded != 0) {
    for (i = 0; i < plugins.loaded; i++) {
      lua_close(plugins.plugins[i]);
    }
    free(plugins.plugins);
  }
}
const char *plugin_get_name(lua_State *L) {
  int t = lua_getglobal(L, "NAME");
  return lua_tostring(L, 1);
}