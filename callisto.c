/*
 * Callisto, a featureful runtime for Lua 5.4.
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "callisto.h"

static const luaL_Reg loadedlibs[] = {
	{CALLISTO_CLLIBNAME,   luaopen_cl},
	{CALLISTO_ENVLIBNAME,  luaopen_environ},
	{CALLISTO_FSYSLIBNAME, luaopen_fs},
	{CALLISTO_JSONLIBNAME, luaopen_json},
	{CALLISTO_MATHLIBNAME, luaopen_math},
	{CALLISTO_OSLIBNAME,   luaopen_os},
	{CALLISTO_PATHLIBNAME, luaopen_path},
	{CALLISTO_PROCLIBNAME, luaopen_process},
	{CALLISTO_SOCKLIBNAME, luaopen_socket},
	{NULL, NULL}
};

lua_State *
callisto_newstate(void)
{
	lua_State *L = luaL_newstate();
	callisto_openlibs(L);
	callisto_setversion(L);
	return L;
}

void
callisto_openall(lua_State *L)
{
	const luaL_Reg *lib;

	/* for each Callisto library except extra */
	for (lib = loadedlibs; lib->func; lib++) {
		lua_newtable(L); /* make a new table for the library */
		lib->func(L); /* load library */
		lua_setglobal(L, lib->name);
	}
	/* inject extra into the global environment */
	luaopen_extra(L);
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) { /* for each key in the extra library */
		/* if the key is not a string, move on to the next one */
		if (!lua_isstring(L, -2)) {
			lua_pop(L, 1);
			continue;
		}
		/* assign the value to a global */
		lua_setglobal(L, lua_tostring(L, -2));
	}
}

void
callisto_openlibs(lua_State *L)
{
	luaL_openlibs(L);
	callisto_openall(L);
}

void
callisto_setversion(lua_State *L)
{
	lua_pushliteral(L, CALLISTO_VERSION);
	lua_setglobal(L, "_CALLISTOVERSION");
}
