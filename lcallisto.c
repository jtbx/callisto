/*
 * Callisto, a featureful runtime for Lua 5.4.
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "lcallisto.h"

/* Callisto libraries */
#include "lcl.h"
#include "lenvironment.h"
#include "lfile.h"
#include "ljson.h"
#include "lmath.h"
#include "los.h"
#include "lprocess.h"
#include "lsocket.h"


static const luaL_Reg loadedlibs[] = {
	{CALLISTO_CLLIBNAME,   callistoopen_cl},
	{CALLISTO_ENVLIBNAME,  callistoopen_environment},
	{CALLISTO_FILELIBNAME, callistoopen_file},
	{CALLISTO_JSONLIBNAME, callistoopen_json},
	{CALLISTO_MATHLIBNAME, callistoopen_math},
	{CALLISTO_OSLIBNAME,   callistoopen_os},
	{CALLISTO_PROCLIBNAME, callistoopen_process},
	{CALLISTO_SOCKLIBNAME, callistoopen_socket},
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
callisto_openlibs(lua_State *L)
{
	const luaL_Reg *lib;

	luaL_openlibs(L);

	/* "require" functions from 'loadedlibs' and set results to global table */
	for (lib = loadedlibs; lib->func; lib++) {
		lua_newtable(L);
		lib->func(L); /* load library */
		lua_setglobal(L, lib->name);
		//lua_pop(L, 1);  /* remove lib */
	}
}

void
callisto_setversion(lua_State *L)
{
	lua_pushliteral(L, CALLISTO_VERSION);
	lua_setglobal(L, "_CALLISTOVERSION");
}
