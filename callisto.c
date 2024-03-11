/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

#include <lua/lauxlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>

#include "callisto.h"

int luaopen_cl(lua_State *);
int luaopen_environ(lua_State *);
int luaopen_extra(lua_State *);
int luaopen_fs(lua_State *);
int luaopen_json(lua_State *);
int luaopen_process(lua_State *);

/* clang-format off */
static const luaL_Reg loadedlibs[] = {
	{ CALLISTO_CLLIBNAME,   luaopen_cl      },
	{ CALLISTO_ENVLIBNAME,  luaopen_environ },
	{ CALLISTO_FSYSLIBNAME, luaopen_fs      },
	{ CALLISTO_JSONLIBNAME, luaopen_json    },
	{ CALLISTO_PROCLIBNAME, luaopen_process },
	{ NULL,                 NULL            }
};
/* clang-format on */

lua_State *
callisto_newstate(void)
{
	lua_State *L;

	L = luaL_newstate();
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
		lib->func(L);    /* load library */
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
