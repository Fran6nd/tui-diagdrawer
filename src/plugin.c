#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "plugin.h"

struct plugin{
    lua_State *plugin;
    char* name; 
};

int plugin_load(char* path) {
  int status, result, i;
  double sum;
  lua_State *L;

  L = luaL_newstate();

  luaL_openlibs(L);

  status = luaL_loadfile(L, path);
  if (status) {
    fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
    exit(1);
  }

  /* Ask Lua to run our little script */
  result = lua_pcall(L, 0, LUA_MULTRET, 0);
  if (result) {
    fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
    exit(1);
  }
}
void plugins_load(){
    plugins.loaded = 0;
    plugin_load("lua/custom_mode.lua");
}
void plugins_free(){
    int i;
    for(i = 0; i < plugins.loaded; i++){
        lua_close(plugins.plugins[i]);
    }
}
const char* plugin_get_name(lua_State *L){
  int t = lua_getglobal(L, "NAME");
  return lua_tostring (L,1);
}