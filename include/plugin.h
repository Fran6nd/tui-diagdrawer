#ifndef PLUGIN_H
#define PLUGIN_H

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

struct Plugins{
lua_State ** plugins;
int loaded;
};
struct Plugins plugins;
void plugins_load();
void plugins_free();

#endif