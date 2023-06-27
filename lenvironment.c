/***
 * Getting and setting
 * environment variables.
 * @module environment
 */

#include <stdlib.h>
#include <errno.h>
#ifdef BSD
#	include <string.h>
#endif

#include <lua.h>
#include <lauxlib.h>

#include "lenvironment.h"
#include "lcallisto.h"
#include "util.h"


/***
 * Returns the value of the given environment
 * variable.
 *
 * If *var* does not exist in the environment
 * and *defaultvalue* is not nil,
 * *defaultvalue* will be returned.
 *
 * @function get
 * @usage environment.get("HOME", "/root")
 * @tparam       string var The environment variable to get.
 * @param[opt] defaultvalue The value to return in case the environment variable was not found.
 */
static int
environment_get(lua_State *L)
{
	const char *variable;
	char *ret;

	variable = luaL_checkstring(L, 1);
	ret = getenv(variable);

	if (ret == NULL)
		if (!lua_isnoneornil(L, 2))
			lua_pushvalue(L, 2);
		else
			lua_pushnil(L);
	else
		lua_pushstring(L, ret);
	return 1;
}

/***
 * Sets the value of the given environment variable.
 *
 * This function will throw an error if the value of
 * *var* is invalid for the name of an environment
 * variable, or if there is insufficient memory
 * available to perform the operation.
 *
 * @function set
 * @usage environment.set("MYVAR", "1")
 * @tparam string   var The environment variable to set.
 * @tparam string value The value to set for *var*.
 */
static int
environment_set(lua_State *L)
{
	int ret;
	const char *variable;
	const char *value;

	variable = luaL_checkstring(L, 1);
	value    = luaL_checkstring(L, 2);
	ret      = setenv(variable, value, 1);

	if (ret == 0)
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

/***
 * Clears the environment.
 *
 * If *var* is not nil, only clears the value of that
 * environment variable (not the whole environment).
 *
 * This function will throw an error if the value of
 * *var* is invalid for the name of an environment
 * variable, or if there is insufficient memory
 * available to perform the operation.
 *
 * @function clear
 * @usage environment.clear("MYVAR")
 * @tparam[opt] string var The environment variable to clear.
 */
static int
environment_clear(lua_State *L)
{
	const char *variable;
	int ret;

	if (lua_isnoneornil(L, 1)) {
		clearenv();
		return 0;
	}

	variable = luaL_checkstring(L, 1);
	ret = unsetenv(variable);

	if (ret == 0)
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

static const luaL_Reg envlib[] = {
	{"get",   environment_get},
	{"set",   environment_set},
	{"clear", environment_clear},
	{NULL, NULL}
};

int
callistoopen_environment(lua_State *L)
{
	luaL_newlib(L, envlib);
	return 1;
}
