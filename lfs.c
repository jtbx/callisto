/***
 * Files, directories and file system manipulation.
 *
 * @module fs
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua.h>
#include <lauxlib.h>

#include "callisto.h"
#include "errors.h"
#include "util.h"

/***
 * Copies the contents of the file *source* to
 * the file *target*. *target* will be overwritten
 * if it already exists.
 *
 * @function copy
 * @usage
io.output("hello"):write("hello world")
fs.copy("hello", "world")
assert(io.input("world"):read("a") == "hello world")
 * @tparam string source The source file to copy.
 * @tparam string target The destination file.
 */
static int
fs_copy(lua_State *L)
{
	struct stat sb;
	const char *source, *target;
	char readbuf[512];
	ssize_t ret;
	int sfd, tfd;

	source = luaL_checkstring(L, 1);
	target = luaL_checkstring(L, 2);

	/* get the source file's mode */
	if (stat(source, &sb) == -1)
		return lfail(L);

	sfd = open(source, O_RDONLY);
	tfd = open(target, O_WRONLY | O_CREAT | O_TRUNC, sb.st_mode);

	if (sfd == -1 || tfd == -1)
		return lfail(L);

	for (;;) {
		ret = read(sfd, readbuf, 512);
		switch (ret) {
		case 0: /* end-of-file */
			goto finish;
			break;
		case -1: /* error */
			close(sfd);
			close(tfd);
			return lfail(L);
			break;
		}

		ret = write(tfd, readbuf, ret);
		if (ret == -1) {
			close(sfd);
			close(tfd);
			return lfail(L);
		}
	}

finish:
	close(sfd);
	close(tfd);

	lua_pushboolean(L, 1);
	return 1;
}

/***
 * Returns true if the given pathname exists
 * in the file system, or false if it does not.
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
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
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

	dir = strdup(luaL_checkstring(L, 1));

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
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
 *
 * @function move
 * @usage fs.move("srcfile", "destfile")
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
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
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
 * directory to *dir*.
 *
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
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
	{"copy",     fs_copy},
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
