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

#include <time.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

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

static const luaL_Reg extlib[] = {
	{"sleep", extra_sleep},
	{NULL, NULL}
};

int
luaopen_extra(lua_State *L)
{
	luaL_newlib(L, extlib);
	return 1;
}
