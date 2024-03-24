/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/***
 * Files, directories and file system manipulation.
 *
 * @module fs
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "util.h"

/* wrapped DIR structure with the
 * absolute path of the directory */
struct wdir {
	DIR *d;
	const char *path;
};

/* clang-format off */
struct {
	mode_t mode;
	char name[8];
} modes[] = {
	/* file types */
	{S_IFMT, "IFMT"},
	{S_IFBLK, "IFBLK"},
	{S_IFCHR, "IFCHR"},
	{S_IFIFO, "IFIFO"},
	{S_IFREG, "IFREG"},
	{S_IFDIR, "IFDIR"},
	{S_IFLNK, "IFLNK"},
	{S_IFSOCK, "IFSOCK"},

	/* file permissions */
	{S_IRWXU, "IRWXU"},
	{S_IRUSR, "IRUSR"},
	{S_IWUSR, "IWUSR"},
	{S_IXUSR, "IXUSR"},
	{S_IRWXG, "IRWXG"},
	{S_IRGRP, "IRGRP"},
	{S_IWGRP, "IWGRP"},
	{S_IXGRP, "IXGRP"},
	{S_IRWXO, "IRWXO"},
	{S_IROTH, "IROTH"},
	{S_IWOTH, "IWOTH"},
	{S_IXOTH, "IXOTH"},
	{S_ISUID, "ISUID"},
	{S_ISGID, "ISGID"},
	{S_ISVTX, "ISVTX"},
	{0, {0}}
};
/* clang-format on */

/***
 * Returns the last component of the given path,
 * removing any trailing '/' characters. If the given
 * path consists entirely of '/' characters, the string
 * `"/"` is returned. If *path* is an empty string,
 * the string `"."` is returned.
 *
 * This is purely a string manipulation function and
 * depends on no outside state.
 *
 * @function basename
 * @usage
print("name of script file is " .. fs.basename(arg[0]))
assert(fs.basename("/etc/fstab") == "fstab")
 * @tparam string path The path to process.
 */
static int
fs_basename(lua_State *L)
{
	const char *ret;
	char *path; /* parameter 1 (string) */

	path = strndup(luaL_checkstring(L, 1), lua_rawlen(L, 1));
	ret = basename(path);

	if (ret == NULL) /* failed? */
		return lfail(L);

	lua_pushstring(L, ret);

	free(path);
	return 1;
}

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
	const char *source; /* parameter 1 (string) */
	const char *target; /* parameter 2 (string) */
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
 * Returns the parent directory of the pathn
 * given. Any trailing '/' characters are not
 * counted as part of the directory name.
 * If the given path is an empty string or contains
 * no '/' characters, the string `"."` is returned,
 * signifying the current directory.
 *
 * This is purely a string manipulation function and
 * depends on no outside state.
 *
 * @function dirname
 * @usage assert(fs.dirname("/usr/local/bin") == "/usr/local")
 * @tparam string path The path to process.
 */
static int
fs_dirname(lua_State *L)
{
	const char *ret;
	char *path; /* parameter 1 (string) */

	path = strndup(luaL_checkstring(L, 1), lua_rawlen(L, 1));
	ret = dirname(path);

	if (ret == NULL) /* failed? */
		return lfail(L);

	lua_pushstring(L, ret);

	free(path);
	return 1;
}

static int fs_status(lua_State *);

static int
entries_iter(lua_State *L)
{
	struct wdir *w;
	struct dirent *ent;
	char cwd[PATH_MAX];

	w = (struct wdir *)lua_touserdata(L, lua_upvalueindex(1));

	ent = readdir(w->d);
	if (!ent) {
		closedir(w->d);
		w->d = NULL;
		return 0;
	}

	getcwd(cwd, PATH_MAX);
	if (chdir(w->path) == -1)
		return lfail(L);

	lua_pushcfunction(L, fs_status);
	lua_pushstring(L, ent->d_name);
	lua_call(L, 1, 1);

	if (chdir(cwd) == -1)
		return lfail(L);

	return 1;
}

static int
entries_gc(lua_State *L)
{
	struct wdir *w;

	w = (struct wdir *)lua_touserdata(L, 1);

	if (w->d)
		closedir(w->d);

	return 0;
}

static int
fs_entries(lua_State *L)
{
	struct wdir *w;
	const char *path; /* parameter 1 (string) */

	path = luaL_optstring(L, 1, ".");
	w = (struct wdir *)lua_newuserdatauv(L, sizeof(struct wdir), 0);
	w->d = opendir(path);
	w->path = path;

	if (!(w->d))
		return luaL_error(L, "%s: %s", path, strerror(errno));

	if (luaL_newmetatable(L, "directory entry")) {
		lua_pushcfunction(L, entries_gc);
		lua_setfield(L, -2, "__gc");
	}

	lua_setmetatable(L, -2);
	lua_pushcclosure(L, entries_iter, 1);

	return 1;
}

/***
 * Returns true if the given pathname exists
 * in the file system.
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
	const char *path; /* parameter 1 (string) */
	int ret;

	path = luaL_checkstring(L, 1);
	ret = access(path, F_OK); /* check if file exists */

	if (ret == -1) /* failed? */
		return lfail(L);

	lua_pushboolean(L, ret == 0);
	return 1;
}

static int
ismode(lua_State *L, mode_t mode)
{
	struct stat sb;
	const char *path; /* parameter 1 (string) */

	path = luaL_checkstring(L, 1);

	if (stat(path, &sb) != -1) {
		lua_pushboolean(L, sb.st_mode & mode);
		return 1;
	}

	switch (errno) {
	case ENOTDIR:
	case ENOENT:
		lua_pushboolean(L, 0);
		return 1;
		break;
	default:
		return lfail(L);
	}
}

/***
 * Returns true if the given path specifies a directory.
 * Will return false if either the given path does not
 * specify a directory or the path does not exist at all.
 *
 * On error returns nil, an error message and a
 * platform-dependent error code.
 *
 * @function isdirectory
 * @usage
if not fs.isdirectory("/usr") then
    print("something's wrong")
end
 * @tparam string path The path to check.
 */
static int
fs_isdirectory(lua_State *L)
{
	return ismode(L, S_IFDIR);
}

/***
 * Returns true if the given path specifies a file.
 * Will return false if either the given path
 * does not specify a directory or the path
 * does not exist at all.
 *
 * On error returns nil, an error message and a
 * platform-dependent error code.
 *
 * @function isfile
 * @usage
if not fs.isfile("/sbin/init") then
    print("something's wrong")
end
 * @tparam string path The path to check.
 */
static int
fs_isfile(lua_State *L)
{
	return ismode(L, S_IFREG);
}

/***
 * Creates a new directory, creating intermediate directories as required.
 *
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
 *
 * @function mkpath
 * @usage fs.mkpath("/usr/local/bin")
 * @tparam string path The path to create.
 */
static int
fs_mkpath(lua_State *L)
{
	/*
	 * Code derived from OpenBSD mkdir(1)
	 */
	struct stat sb;
	const char *path; /* path to make */
	char *slash;
	mode_t mode;     /* file mode of terminal directory */
	mode_t dir_mode; /* file mode of intermediate directories */
	int done;

	path = luaL_checkstring(L, 1);
	mode = dir_mode = 0777;
	slash = (char *)path;

	for (;;) {
		slash += strspn(slash, "/");
		slash += strcspn(slash, "/");

		done = (*slash == '\0');
		*slash = '\0';

		if (mkdir(path, done ? mode : dir_mode) == 0) {
			if (mode > 0777 && chmod(path, mode) == -1)
				return lfail(L);
		} else {
			int mkdir_errno = errno;

			if (stat(path, &sb) == -1) {
				/* Not there; use mkdir()s errno */
				errno = mkdir_errno;
				return lfail(L);
			}
			if (!S_ISDIR(sb.st_mode)) {
				/* Is there, but isn't a directory */
				errno = ENOTDIR;
				return lfail(L);
			}
		}

		if (done)
			break;

		*slash = '/';
	}

	lua_pushboolean(L, 1);
	return 1;
}

/***
 * Creates a new directory.
 *
 * If you need to create multiple directories at once (similar to
 * the behaviour of `mkdir -p`), use *fs.mkpath* instead.
 *
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
 *
 * @function mkdir
 * @usage fs.mkdir("/usr/local/bin")
 * @tparam string dir The path of the directory to create.
 */
static int
fs_mkdir(lua_State *L)
{
	char *dir; /* parameter 1 (string) */
	int ret;

	dir = strdup(luaL_checkstring(L, 1));
	ret = mkdir(dir, 0777);
	free(dir);

	if (ret == 0) {
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
}

/***
 * Moves the item at path *src* to path *dest*.
 * If *dest* exists, it is overwritten. Both *src* and *dest*
 * must be of the same type (that is, both directories or both
 * non-directories) and must reside on the same file system.
 *
 * (If you need to move files across different file systems,
 * consider using `fs.copy` instead.)
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

	src = luaL_checkstring(L, 1);
	dest = luaL_checkstring(L, 2);
	ret = rename(src, dest); /* move file */

	if (ret == 0) { /* check for success */
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
}

static int
recursiveremove(lua_State *L, const char *path)
{
	DIR *d;
	struct dirent *ent;
	char *fullname;
	size_t len, nlen, plen;

	if ((d = opendir(path)) == NULL) {
		if (errno != ENOTDIR)
			return lfail(L);

		/* if it's a file... */
		if (remove(path) == -1)
			return lfail(L);

		lua_pushboolean(L, 1);
		return 1;
	}

	while ((ent = readdir(d)) != NULL) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;

		plen = strlen(path);
		nlen = strlen(ent->d_name);
		/* path + name + slash in between + null terminator */
		len = plen + nlen + 2;
		fullname = malloc(len * sizeof(char));

		strbcpy(fullname, path, len);
		fullname[plen] = '/';
		fullname[plen + 1] = 0;
		strbcat(fullname, ent->d_name, len);

		if (ent->d_type == DT_DIR) {
			/* if rmdir succeeded, free fullname and
			 * proceed with next entry */
			if (rmdir(fullname) != -1)
				goto next;

			if (errno == ENOTEMPTY) /* if the directory is not empty... */
				recursiveremove(L, fullname);
			else /* if some other error occurred */
				return lfail(L);
		} else {
			if (remove(fullname) == -1)
				return lfail(L);
		}

next:
		free(fullname);
	}

	closedir(d);
	if (rmdir(path) == -1)
		return lfail(L);

	lua_pushboolean(L, 1);
	return 1;
}

/***
 * Removes a file or directory.
 *
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
 *
 * @function remove
 * @usage fs.remove("path/to/file")
 * @tparam string dir The path to remove.
 */
static int
fs_remove(lua_State *L)
{
	const char *path; /* parameter 1 (string) */

	path = luaL_checkstring(L, 1);

	return recursiveremove(L, path);
}

/***
 * Removes an empty directory.
 *
 * On success, returns true. Otherwise returns nil, an error
 * message and a platform-dependent error code.
 *
 * @function rmdir
 * @usage fs.rmdir("yourdirectory")
 * @tparam string dir The path of the directory to remove.
 */
static int
fs_rmdir(lua_State *L)
{
	const char *dir; /* parameter 1 (string) */
	int ret;

	dir = luaL_checkstring(L, 1);
	ret = rmdir(dir);

	if (ret == 0) {
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
}

static int
fs_status(lua_State *L)
{
	struct stat s;
	const char *path; /* parameter 1 (string) */

	path = luaL_checkstring(L, 1);

	if (lstat(path, &s) == -1)
		return lfail(L);

	lua_createtable(L, 0, 7);

	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "path");

	lua_pushinteger(L, s.st_mode);
	lua_setfield(L, -2, "mode");

	lua_pushinteger(L, s.st_uid);
	lua_setfield(L, -2, "uid");
	lua_pushinteger(L, s.st_gid);
	lua_setfield(L, -2, "gid");

	lua_pushinteger(L, s.st_atim.tv_sec);
	lua_setfield(L, -2, "accessdate");
	lua_pushinteger(L, s.st_mtim.tv_sec);
	lua_setfield(L, -2, "modifydate");
	lua_pushinteger(L, s.st_ctim.tv_sec);
	lua_setfield(L, -2, "chdate");

	return 1;
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
	char buffer[512];    /* buffer used by getcwd() */
	char *ret;

	if (lua_isnoneornil(L, 1)) { /* if first argument was not given... */
		ret = getcwd(buffer, 512);

		if (ret != NULL) {
			lua_pushstring(L, buffer);
			return 1;
		}

		return lfail(L);
	}

	workdir = luaL_checkstring(L, 1);

	if (chdir(workdir) == 0) {
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
}

/* clang-format off */

static const luaL_Reg fslib[] = {
	{"basename",    fs_basename},
	{"copy",        fs_copy},
	{"dirname",     fs_dirname},
	{"entries",     fs_entries},
	{"exists",      fs_exists},
	{"isdirectory", fs_isdirectory},
	{"isfile",      fs_isfile},
	{"mkdir",       fs_mkdir},
	{"mkpath",      fs_mkpath},
	{"move",        fs_move},
	{"remove",      fs_remove},
	{"rmdir",       fs_rmdir},
	{"status",      fs_status},
	{"workdir",     fs_workdir},
	{NULL, NULL}
};

int
luaopen_fs(lua_State *L)
{
	int i;

	luaL_newlib(L, fslib);

	for (i = 0; modes[i].name[0] != 0; i++) {
		lua_pushinteger(L, modes[i].mode);
		lua_setfield(L, -2, modes[i].name);
	}

	return 1;
}
