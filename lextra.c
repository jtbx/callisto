/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/***
 * Extra functions that don't fit into any other category.
 *
 * This module does not provide its own table;
 * all functions here are found in the global table.
 *
 * @module extra
 */

#include <string.h>
#include <time.h>
#include <unistd.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "util.h"

static int
extra_printfmt(lua_State *L)
{
	const char *outstring;
	int i, nargs;

	nargs = lua_gettop(L);

	lua_pushcfunction(L, lstrfmt);
	for (i = 1; i <= nargs; i++) {
		lua_pushvalue(L, i);
	}
	lua_call(L, nargs, 1);

	outstring = lua_tostring(L, -1);
	lua_pop(L, 1);
	write(1, outstring, strlen(outstring));
	write(1, "\n", 1);

	return 0;
}

/***
 * Waits the specified amount of seconds.
 *
 * @function sleep
 * @usage
local minutes = 5
sleep(minutes * 60) -- 5 minutes
 * @tparam number seconds The amount of seconds to wait.
 */
static int
extra_sleep(lua_State *L)
{
	/* Implementation from luasocket */
	double n;
	struct timespec t, r;

	n = luaL_checknumber(L, 1);

	if (n < 0.0)
		n = 0.0;
	if (n > INT_MAX)
		n = INT_MAX;

	t.tv_sec = (int)n;
	n -= t.tv_sec;
	t.tv_nsec = (int)(n * 1000000000);

	if (t.tv_nsec >= 1000000000)
		t.tv_nsec = 999999999;

	while (nanosleep(&t, &r) != 0) {
		t.tv_sec = r.tv_sec;
		t.tv_nsec = r.tv_nsec;
	}
	return 0;
}

/* clang-format off */

static const luaL_Reg extlib[] = {
	{"printfmt", extra_printfmt},
	{"sleep",    extra_sleep},
	{NULL, NULL}
};

int
luaopen_extra(lua_State *L)
{
	luaL_newlib(L, extlib);
	return 1;
}
