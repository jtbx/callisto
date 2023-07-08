/***
 * Getting and setting environment variables.
 * @module environ
 */

#include <stdlib.h>
#include <errno.h>

#include <lua.h>
#include <lauxlib.h>

#include "lcallisto.h"


extern char **environ;

/***
 * Table enabling easy access to environment
 * variables.
 *
 * Has metamethods *\_\_index* and *\_\_newindex*,
 * which allow for the table to be indexed with a
 * string to get values of environment variables,
 * or for fields to be set to set an environment
 * variable.
 *
 * @table environ
 * @usage
-- Print the value of an environment variable:
print(environ["MYVAR"])
-- Set an environment variable:
environ["MYVAR"] = 1
 */

/*
 * Returns the value of the given environment variable.
 */
static int
environ_index(lua_State *L)
{
	const char *variable; /* parameter 2 (string) */
	char *ret;

	variable = luaL_checkstring(L, 2);
	ret = getenv(variable);

	if (ret == NULL) /* no environment variable? */
		lua_pushnil(L);
	else
		lua_pushstring(L, ret); /* push variable value */

	return 1;
}

/*
 * Sets the value of the given environment variable.
 */
static int
environ_newindex(lua_State *L)
{
	int ret;
	const char *variable; /* parameter 2 (string) */
	const char *value;    /* parameter 3 (string) */

	variable = luaL_checkstring(L, 2);

	if (lua_isnil(L, 3)) {
		ret = unsetenv(variable); /* remove variable from environ */
		if (ret == 0) /* did unsetenv succeed? */
			return 0;

		/* if unsetenv didn't succeed:
		 * (unsetenv only sets errno to EINVAL on error) */
		return luaL_error(L, "invalid input string");
	}

	value = luaL_checkstring(L, 3);
	ret	= setenv(variable, value, 1);

	if (ret == 0) /* did setenv succeed? */
		return 0;

	switch (errno) {
		case EINVAL:
			return luaL_error(L, "invalid input string");
			break;
		case ENOMEM:
			return luaL_error(L, "insufficient memory");
			break;
	}

	return 0;
}

static const luaL_Reg mt[] = {
	{"__index",    environ_index},
	{"__newindex", environ_newindex},
	{NULL, NULL}
};

int
luaopen_environ(lua_State *L)
{
	const luaL_Reg *lib;

	lua_newtable(L);
	luaL_newmetatable(L, CALLISTO_ENVIRON);  /* metatable for environ */

	for (lib = mt; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	lua_setmetatable(L, -2);
	return 1;
}
