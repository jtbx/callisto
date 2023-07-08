/*
 * Callisto, a featureful runtime for Lua 5.4.
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "lcallisto.h"


static const luaL_Reg loadedlibs[] = {
	{CALLISTO_CLLIBNAME,   luaopen_cl},
	{CALLISTO_ENVLIBNAME,  luaopen_environ},
	{CALLISTO_EXTLIBNAME,  luaopen_extra},
	{CALLISTO_FILELIBNAME, luaopen_file},
	{CALLISTO_JSONLIBNAME, luaopen_json},
	{CALLISTO_MATHLIBNAME, luaopen_math},
	{CALLISTO_OSLIBNAME,   luaopen_os},
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

	/* for every Callisto library */
	for (lib = loadedlibs; lib->func; lib++) {
		lua_newtable(L); /* make a new table for the library */
		lib->func(L); /* load library */
		lua_setglobal(L, lib->name);
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
