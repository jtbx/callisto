/***
 * Math and algorithms.
 * @module math
 */

#include <lua.h>
#include <lauxlib.h>

#include "lmath.h"
#include "lcallisto.h"
#include "util.h"


static int
math_lerp(lua_State *L)
{
	double x; /* parameter 1 (number) */
	double y; /* parameter 2 (number) */
	double z; /* parameter 3 (number) */

	x = luaL_checknumber(L, 1);
	y = luaL_checknumber(L, 2);
	z = luaL_checknumber(L, 3);

	lua_pushnumber(L, x + (y - x) * z);
	return 1;
}

static const luaL_Reg mathlib[] = {
	{"lerp", math_lerp},
	{NULL, NULL}
};

int
callistoopen_math(lua_State *L)
{
	newoverride(L, mathlib, CALLISTO_MATHLIBNAME);
	return 0;
}
