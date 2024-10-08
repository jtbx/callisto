/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/***
 * Getting and setting environment variables.
 * @module environ
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "callisto.h"
#include "util.h"

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
environ__index(lua_State *L)
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
environ__newindex(lua_State *L)
{
	int ret;
	const char *variable; /* parameter 2 (string) */
	const char *value;    /* parameter 3 (string) */

	variable = luaL_checkstring(L, 2);

	if (lua_isnil(L, 3)) {
		ret = unsetenv(variable); /* remove variable from environ */
		if (ret == 0)             /* did unsetenv succeed? */
			return 0;

		/* if unsetenv didn't succeed:
		 * (unsetenv only sets errno to EINVAL on error) */
		return luaL_error(L, "invalid input string");
	}

	value = luaL_checkstring(L, 3);
	ret = setenv(variable, value, 1);

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

static int
pairs_iter(lua_State *L)
{
	char *line, *name;
	size_t *envi, len;
	int i;

	len = 0;
	envi = (size_t *)lua_touserdata(L, 1);
	line = environ[*envi];

	if (line == NULL) {
		lua_pushnil(L);
		return 1;
	}

	name = calloc(strlen(line), sizeof(char));

	/* extract the name portion from the environ line */
	for (i = 0; line[i] != '='; i++) {
		if (line[i] == '\0')
			return lfailm(L, "process environment is corrupted");

		name[len] = line[i];
		len++;
	}

	(*envi)++;
	lua_pushlstring(L, name, len);
	return 1;
}

static int
environ__pairs(lua_State *L)
{
	size_t *p;

	lua_pushcfunction(L, pairs_iter);
	p = lua_newuserdatauv(L, sizeof(size_t), 0);
	*p = 0;
	return 2;
}

/* clang-format off */

static const luaL_Reg mt[] = {
	{"__index",    environ__index},
	{"__newindex", environ__newindex},
	{"__pairs",    environ__pairs},
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
