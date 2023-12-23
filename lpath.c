/***
 * File system path manipulation.
 *
 * @module path
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>

#include "callisto.h"
#include "util.h"

/***
 * Returns the last component of the given pathname,
 * removing any trailing '/' characters. If the given
 * path consists entirely of '/' characters, the string
 * `"/"` is returned. If *path* is an empty string,
 * the string `"."` is returned.
 *
 * This function may return nil if the given
 * pathname exceeds the system's path length
 * limit (on most Linux systems this will be 4096).
 *
 * @function basename
 * @usage path.basename(arg[0])
 * @tparam string path The path to process.
 */
static int
path_basename(lua_State *L)
{
	const char *ret;
	char *path; /* parameter 1 (string) */

	path = strndup(luaL_checkstring(L, 1), lua_rawlen(L, 1));
	ret  = basename(path);

	if (ret == NULL) /* failed? */
		return lfail(L);

	lua_pushstring(L, ret);

	free(path);
	return 1;
}

/***
 * Returns the parent directory of the pathname
 * given. Any trailing '/' characters are not
 * counted as part of the directory name.
 * If the given path is an empty string or contains
 * no '/' characters, the string `"."` is returned,
 * signifying the current directory.
 *
 * This function may return nil if the given
 * pathname exceeds the system's path length
 * limit (on most Linux systems this will be 4096).
 *
 * @function dirname
 * @usage path.dirname(arg[0])
 * @tparam string path The path to process.
 */
static int
path_dirname(lua_State *L)
{
	const char *ret;
	char *path; /* parameter 1 (string) */

	path = strndup(luaL_checkstring(L, 1), lua_rawlen(L, 1));
	ret  = dirname(path);

	if (ret == NULL) /* failed? */
		return lfail(L);

	lua_pushstring(L, ret);

	free(path);
	return 1;
}

static const luaL_Reg pathlib[] = {
	{"basename", path_basename},
	{"dirname",  path_dirname},
	{NULL, NULL}
};

int
luaopen_path(lua_State *L)
{
	luaL_newlib(L, pathlib);
	return 1;
}
