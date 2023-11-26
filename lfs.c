/***
 * Files, directories and file system manipulation.
 *
 * @module fs
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fts.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef BSD
#	include <string.h>
#endif
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>

#include "callisto.h"
#include "errors.h"
#include "util.h"

/***
 * Returns true if the given pathname exists
 * in the file system, or returns false if it
 * does not.
 *
 * This function may throw an error if the given
 * pathname exceeds the system's path length
 * limit (on most Linux systems this will be 4096).
 *
 * @function exists
 * @usage
if fs.exists("/etc/fstab") then
	-- ...
end
 * @tparam string path The path of the file to look for.
 */
static int
fs_exists(lua_State *L)
{
	const char *path;
	int ret;

	path = luaL_checkstring(L, 1);
	ret = access(path, F_OK); /* check if file exists */

	if (ret == -1) /* failed? */
		return lfail(L);

	lua_pushboolean(L, ret == 0);
	return 1;
}

/*
 * Taken from OpenBSD mkdir(1)
 * mkpath -- create directories.
 *	path     - path
 *	mode     - file mode of terminal directory
 *	dir_mode - file mode of intermediate directories
 */
static int
mkpath(char *path, mode_t mode, mode_t dir_mode)
{
	struct stat sb;
	char *slash;
	int done;

	slash = path;

	for (;;) {
		slash += strspn(slash, "/");
		slash += strcspn(slash, "/");

		done = (*slash == '\0');
		*slash = '\0';

		if (mkdir(path, done ? mode : dir_mode) == 0) {
			if (mode > 0777 && chmod(path, mode) == -1)
				return -1;
		} else {
			int mkdir_errno = errno;

			if (stat(path, &sb) == -1) {
				/* Not there; use mkdir()s errno */
				errno = mkdir_errno;
				return -1;
			}
			if (!S_ISDIR(sb.st_mode)) {
				/* Is there, but isn't a directory */
				errno = ENOTDIR;
				return -1;
			}
		}

		if (done)
			break;

		*slash = '/';
	}

	return 0;
}

/***
 * Creates a new directory.
 *
 * If *recursive* is true, creates intermediate directories
 * as required; behaves as POSIX `mkdir -p` would.
 *
 * On success, returns true. Otherwise returns nil plus
 * an error message.
 *
 * This function will return nil plus an error message
 * if one of the following conditions are met:
 *
 *  - A component of one of the given pathnames is not a directory
 *
 *  - The given pathname exceeds the system's path length limit
 *
 *  - A component of the path prefix does not exist
 *
 *  - Search permission is denied for a component of the path prefix,
 *  or write permission is denied on the parent directory of the
 *  directory to be created.
 *
 *  - The given pathname could not be translated as a result
 *  of too many symbolic links
 *
 *  - The directory to be created resides on a read-only file system
 *
 *  - The pathname given exists as a file
 *
 *  - There is no space left on the file system
 *
 *  - The current user's quota of disk blocks on the file system
 *  containing the parent directory of the directory to be created
 *  has been exhausted
 *
 * @function mkdir
 * @usage fs.mkdir("/usr/local/bin")
 * @tparam string dir The path of the directory to create.
 * @tparam[opt] boolean recursive Whether to create intermediate
 * directories as required.
 */
static int
fs_mkdir(lua_State *L)
{
	char *dir;     /* parameter 1 (string) */
	int recursive; /* parameter 2 (boolean) */
	int ret;

	dir = (char *)luaL_checkstring(L, 1);

	if (!lua_isboolean(L, 2) && !lua_isnoneornil(L, 2))
		luaL_typeerror(L, 2, "boolean");
	recursive = lua_toboolean(L, 2);

	if (recursive)
		ret = mkpath(dir, 0777, 0777);
	else
		ret = mkdir(dir, 0777);
	
	if (ret == 0) {
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
}

/***
 * Moves the file at the path *src* to the path *dest*,
 * moving it between directories if required. If *dest*
 * exists, it is first removed. Both *src* and *dest*
 * must be of the same type (that is, both directories or
 * both non-directories) and must reside on the same file system.
 *
 * This function will return nil plus an error message
 * if one of the following conditions are met:
 *
 *  - One of the given pathnames exceeds the system's path length limit
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
 *  - *src* is a directory, but *dest* is not
 *
 *  - *dest* is a directory, but *src* is not
 *
 *  - The given pathnames are on different file systems
 *
 *  - There is no space left on the file system
 *
 *  - The current user's quota of disk blocks on the file system
 *  containing the directory of *dest* has been exhausted
 *
 *  - The directory containing *dest* resides on a read-only file system
 *
 * @function move
 * @usage fs.move("file1", "file2")
 * @tparam string src  The pathname of the file to move.
 * @tparam string dest The destination pathname.
 */
static int
fs_move(lua_State *L)
{
	int ret;
	const char *src;  /* parameter 1 (string) */
	const char *dest; /* parameter 2 (string) */

	src  = luaL_checkstring(L, 1);
	dest = luaL_checkstring(L, 2);
	ret  = rename(src, dest); /* move file */

	if (ret == 0) { /* check for success */
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);

	return 0;
}

/***
 * Removes an empty directory.
 *
 * On success, returns true. Otherwise returns nil plus
 * an error message.
 *
 * This function will return nil plus an error message
 * if one of the following conditions are met:
 *
 *  - A component of the given path is not a directory
 *
 *  - The given pathname exceeds the system's path length limit
 *
 *  - The named directory does not exist
 *
 *  - The given pathname could not be translated as a result
 *  of too many symbolic links
 *
 *  - The named directory contains files other than '.' and '..' in it
 *
 *  - Search permission is denied for a component of the path prefix,
 *  or write permission is denied on the directory containing the directory
 *  to be removed
 *
 *  - The directory containing the directory to be removed is marked sticky,
 *  and neither the containing directory nor the directory to be removed are
 *  owned by the current user
 *
 *  - The directory to be removed or the directory containing it has its
 *  immutable or append-only flag set
 *
 *  - The directory to be removed is the mount point for a mounted file system
 *
 *  - The last component of the directory to be removed is a '.'
 *
 *  - The directory to be created resides on a read-only file system
 *
 * @function mkdir
 * @usage fs.rmdir("yourdirectory")
 * @tparam string dir The path of the directory to remove.
 */
static int
fs_rmdir(lua_State *L)
{
	const char *dir;
	int ret;

	dir = luaL_checkstring(L, 1);
	ret = rmdir(dir);

	if (ret == 0) {
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);

	return 0;
}

/***
 * Returns or sets the current working directory.
 *
 * If *dir* is nil, returns the current working
 * directory. Otherwise, sets the current working
 * directory to *dir* and returns true on success.
 *
 * This function will return nil and an error
 * message if one of the following conditions are met:
 *
 *  - One of the given pathnames exceeds the system's path length limit
 *
 *  - The current user is denied permission to perform the action
 *
 *  - The current working directory or *dir* no longer exists
 *
 *  - *dir* is not nil, nor is it the name of a directory
 *
 *  - *dir* could not be translated as a result of too many symbolic links
 *
 * @function workdir
 * @usage
-- Store the current working directory in a variable:
local wd = fs.workdir()
-- Set the current working directory to the value of
-- the HOME environment variable:
fs.workdir(environ["HOME"])
 * @tparam[opt] string dir The directory to set as the current working directory.
 */
static int
fs_workdir(lua_State *L)
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
		return lfail(L);
	} else {
		workdir = luaL_checkstring(L, 1);

		if (chdir(workdir) == 0) {
			lua_pushboolean(L, 1);
			return 1;
		}

		return lfail(L);
	}
}

static const luaL_Reg fslib[] = {
	{"exists",   fs_exists},
	{"mkdir",    fs_mkdir},
	{"move",     fs_move},
	{"rmdir",    fs_rmdir},
	{"workdir",  fs_workdir},
	{NULL, NULL}
};

int
luaopen_fs(lua_State *L)
{
	luaL_newlib(L, fslib);
	return 1;
}
