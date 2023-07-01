/***
 * Files, path manipulation,
 * and permissions.
 * @module file
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#ifdef BSD
#	include <string.h>
#endif

#include <lua.h>
#include <lauxlib.h>

#include "lcallisto.h"
#include "util.h"


/***
 * Returns the last component of the given pathname,
 * removing any trailing '/' characters. If *path*
 * consists entirely of '/' characters, the string
 * `"/"` is returned. If *path* is an empty string,
 * the string `"."` is returned.
 *
 * This function may return nil if the given
 * pathname is longer than the system's path length
 * limit (On most Linux systems this will be 4096).
 *
 * @function basename
 * @usage file.basename(arg[0])
 * @tparam string path The path to process.
 */
static int
file_basename(lua_State *L)
{
	const char *path; /* parameter 1 (string) */
	const char *ret;

	path = luaL_checkstring(L, 1);
	ret  = basename(path);

	if (ret == NULL && errno == ENAMETOOLONG) /* check if path is too long */
		return lfail(L, "pathname too long");

	lua_pushstring(L, ret);
	return 1;
}

/***
 * Returns the parent directory of the pathname
 * given. Any trailing '/' characters are not
 * counted as part of the directory name.
 * If *path* is an empty string or contains no
 * '/' characters, the string `"."` is returned,
 * signifying the current directory.
 *
 * This function may return nil if the given
 * pathname is longer than the system's path length
 * limit (On most Linux systems this will be 4096).
 *
 * @function dirname
 * @usage file.dirname(arg[0])
 * @tparam string path The path to process.
 */
static int
file_dirname(lua_State *L)
{
	const char *path; /* parameter 1 (string) */
	const char *ret;

	path = luaL_checkstring(L, 1);
	ret  = dirname(path);

	if (ret == NULL && errno == ENAMETOOLONG) /* check if path is too long */
		return lfail(L, "pathname too long");

	lua_pushstring(L, ret);
	return 1;
}

/***
 * Returns true if the given pathname exists
 * in the file system, or returns false if it
 * does not.
 *
 * This function may throw an error if the given
 * pathname is longer than the system's path length
 * limit (On most Linux systems this will be 4096).
 *
 * @function exists
 * @usage
if file.exists("/etc/fstab") then
	-- ...
end
 * @tparam string path The path of the file to look for.
 */
static int
file_exists(lua_State *L)
{
	const char *path;
	int ret;

	path = luaL_checkstring(L, 1);
	ret = access(path, F_OK); /* check if file exists */

	if (ret == -1 && errno == ENAMETOOLONG) /* check if path is too long */
		return lfail(L, "pathname too long");

	lua_pushboolean(L, ret == 0);
	return 1;
}

/***
 * Moves the file at the path *from*
 * to the path *to*, moving it between directories
 * if required. If *to* exists, it is first removed.
 * Both *from* and *to* must be of the same type
 * (that is, both directories or both non-directories)
 * and must reside on the same file system.
 *
 * This function will return nil and an error
 * message if one of the following conditions are met:
 *
 *  - One of the given pathnames are longer than the system's path length limit
 *
 *  - One of the given pathnames point to a file that does not exist
 *
 *  - An attempt was made to move a parent directory of a pathname
 *
 *  - The current user is denied permission to perform the action
 *
 *  - One of the given pathnames could not be translated as a result
 *  of too many symbolic links
 *
 *  - A component of one of the given pathnames is not a directory
 *
 *  - *from* is a directory, but *to* is not
 *
 *  - *to* is a directory, but *from* is not
 *
 *  - The given pathnames are on different file systems
 *
 *  - There is no space left on the file system
 *
 *  - The current user's quota of disk blocks on the file system
 *  containing the directory of *to* has been exhausted
 *
 *  - The directory containing *to* resides on a read-only file system
 *
 * This function will throw an error if:
 *
 *  - An I/O error occurred
 *
 *  - An internal memory error occurred
 *
 * @function move
 * @usage file.move("file1", "file2")
 * @tparam string from The pathname of the file to move.
 * @tparam string   to The pathname to move *from* to.
 */
static int
file_move(lua_State *L)
{
	int ret;
	const char *from; /* parameter 1 (string) */
	const char *to;   /* parameter 2 (string) */

	from = luaL_checkstring(L, 1);
	to   = luaL_checkstring(L, 2);
	ret  = rename(from, to); /* move file */

	if (ret == 0) { /* check for success */
		lua_pushboolean(L, 1);
		return 1;
	}

	switch (errno) {
		case ENAMETOOLONG:
			return lfail(L, "pathname too long");
			break;
		case ENOENT:
			return lfail(L, "no such file or directory");
			break;
		case EACCES:
			return lfail(L, "permission denied");
			break;
		case EPERM:
			return lfail(L, "permission denied (sticky directory)");
			break;
		case ELOOP:
			return lfail(L, "could not translate pathname; too many symbolic links");
			break;
		case EMLINK:
			return lfail(L, "maximum link count reached");
			break;
		case ENOTDIR:
			return lfail(L, "component of pathname is not a directory");
			break;
		case EISDIR:
			return lfail(L, "cannot move a file to the name of a directory");
			break;
		case EXDEV:
			return lfail(L, "pathnames are on different file systems");
			break;
		case ENOSPC:
			return lfail(L, "insufficient space left on file system");
			break;
		case EDQUOT:
			return lfail(L, "file system quota reached");
			break;
		case EIO:
			return luaL_error(L, "I/O error");
			break;
		case EROFS:
			return lfail(L, "read-only file system");
			break;
		case EFAULT:
			return luaL_error(L, "internal error (EFAULT)");
			break;
		case EINVAL:
			return lfail(L, "cannot move a parent directory of pathname");
			break;
	}

	return 0;
}

/***
 * Sets/returns the current working directory.
 *
 * If *dir* is nil, returns the current working
 * directory. Otherwise, sets the current working
 * directory to *dir*.
 *
 * This function will return nil and an error
 * message if one of the following conditions are met:
 *
 *  - One of the given pathnames are longer than the system's path length limit
 *
 *  - The current user is denied permission to perform the action
 *
 *  - The current working directory or *dir* no longer exists
 *
 *  - *dir* is not nil, nor is it the name of a directory
 *
 *  - *dir* could not be translated as a result of too many symbolic links
 *
 * This function will throw an error if:
 *
 *  - Insufficient memory is available
 *
 *  - An internal memory error occurred
 *
 *  - An I/O error occurred
 *
 * @function workdir
 * @usage
-- Store the current working directory in a variable:
local wd = file.workdir()
-- Set the current working directory to $HOME:
file.workdir(environment.get("HOME"))
 * @tparam[opt] string dir The directory to set as the current working directory.
 */
static int
file_workdir(lua_State *L)
{
	const char *workdir; /* parameter 1 (string) */
	char *buffer;        /* buffer used by getcwd() */
	char *ret;
	if (lua_isnoneornil(L, 1)) { /* if first argument was not given... */
		buffer = malloc(sizeof(char *) * 512);
		ret = getcwd(buffer, 512);

		if (ret != NULL) {
			lua_pushstring(L, buffer);
			free(buffer);
			return 1;
		}

		free(buffer);

		switch (errno) {
			case EACCES:
				return lfail(L, "permission denied");
				break;
			case EFAULT:
				return luaL_error(L, "internal error (EFAULT)");
				break;
			case ENOENT:
				return lfail(L, "working directory is no longer valid");
				break;
			case ENOMEM:
				return luaL_error(L, "insufficient memory");
				break;
			case ERANGE:
			case ENAMETOOLONG: /* glibc */
				return lfail(L, "pathname too long");
				break;
		}

		return 0;
	} else {
		workdir = luaL_checkstring(L, 1);

		if (chdir(workdir) == 0) {
			lua_pushboolean(L, 1);
			return 1;
		}

		switch (errno) {
			case ENOTDIR:
				return lfail(L, "pathname is not a directory");
				break;
			case ENAMETOOLONG:
				return lfail(L, "pathname too long");
				break;
			case ENOENT:
				return lfail(L, "no such file or directory");
				break;
			case ELOOP:
				return lfail(L, "could not translate pathname; too many symbolic links");
				break;
			case EACCES:
				return lfail(L, "permission denied");
				break;
			case EFAULT:
				return luaL_error(L, "internal error (EFAULT)");
				break;
			case EIO:
				return luaL_error(L, "I/O error");
				break;
		}

		return 0;
	}
}

static const luaL_Reg filelib[] = {
	{"basename", file_basename},
	{"dirname",  file_dirname},
	{"exists",   file_exists},
	{"move",     file_move},
	{"workdir",  file_workdir},
	{NULL, NULL}
};

int
callistoopen_file(lua_State *L)
{
	luaL_newlib(L, filelib);
	return 1;
}
