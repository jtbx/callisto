/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/***
 * Option parsing, and error formatting
 * for the command line.
 * @module cl
 */

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "util.h"

static void
fmesgshift(lua_State *L, int fd, int shift)
{
	int paramc, i;
	char *bname;
	char *progname; /* argv[0] */

	lua_getglobal(L, "arg");
	if (!lua_istable(L, -1)) {
		luaL_error(L, "arg table not present/inaccessible; cannot get script name");
		return;
	}

	lua_geti(L, -1, 0);
	if (!lua_isstring(L, -1)) {
		luaL_argerror(L, 1, "arg[0] is not a string; cannot print script name");
		return;
	}
	progname = (char *)lua_tostring(L, -1);

	/* format using string.format */
	lua_getglobal(L, "string");
	lua_getfield(L, -1, "format");

	paramc = lua_gettop(L); /* get parameter count */
	for (i = 1 + shift; i <= paramc; i++) { /* for every parameter */
		lua_pushvalue(L, i); /* push argument */
	}

	lua_call(L, paramc - shift, 1);

	bname = basename(progname);
	write(fd, bname, strlen(bname));
	write(fd, ": ", 2);
	write(fd, lua_tostring(L, -1), lua_rawlen(L, -1));
	write(fd, "\n", 1);
}

static void
fmesg(lua_State *L, int fd)
{
	fmesgshift(L, fd, 0);
}

/***
 * Prints a formatted error message to standard error.
 * It looks like so:
 *
 * `progname: message`
 *
 * where *progname* is the name of the current script
 * being executed and *message* is the *message* parameter.
 *
 * The *message* parameter may optionally be followed by
 * a variable number of arguments which are formatted
 * using *string.format*.
 *
 * @function error
 * @usage
local succeeded, err = io.open("file.txt")
if not succeeded then
	cl.error("could not open " .. err)
end
 * @tparam string message The message to print after the program's name. Supports *string.format*-style format specifiers.
 * @param  ... Any additional values specified by the format specifiers in the *message* parameter.
 */
static int
cl_error(lua_State *L)
{
	luaL_checkstring(L, 1);

	fmesg(L, 2);

	return 0;
}

/***
 * Prints a formatted message to standard output.
 * It looks like so:
 *
 * `progname: message`
 *
 * where *progname* is the name of the current script
 * being executed and *message* is the (optionally
 * formatted) *message* parameter.
 *
 * The *message* parameter may optionally be followed by
 * a variable number of arguments which are formatted
 * using *string.format*.
 *
 * @function mesg
 * @usage cl.mesg("message to stdout")
 * @tparam string message The message to print after the program's name. Supports *string.format*-style format specifiers.
 * @param  ... Any additional values specified by the format specifiers in the *message* parameter.
 */
static int
cl_mesg(lua_State *L)
{
	luaL_checkstring(L, 1);

	fmesg(L, 1);

	return 0;
}

/***
 * Convenience function that calls *cl.error* to
 * print a formatted error message to standard error,
 * then terminates script execution and calls *os.exit*.
 *
 * For more information on the *message* parameter,
 * see the *cl.error* function.
 *
 * @function panic
 * @usage
local succeeded, err, code = io.open("file.txt")
if not succeeded then
	-- use a custom exit code:
	cl.panic(code, "could not open " .. err)
	-- use the default exit code (1):
	cl.panic("could not open " .. err)
end
 * @tparam[opt] integer code The exit code to return to the operating system.
 * @tparam string message The message to print after the program's name. Supports *string.format*-style format specifiers.
 * @param  ... Any additional values specified by the format specifiers in the *message* parameter.
 */
static int
cl_panic(lua_State *L)
{
	int code, codegiven;

	/* get exit code */
	codegiven = lua_isinteger(L, 1);
	code = codegiven ? lua_tointeger(L, 1) : 1;

	/* check arguments */
	luaL_checkstring(L, 1 + codegiven);
	fmesgshift(L, 2, codegiven);

	/* exit */
	lua_close(L);
	if (L) /* used to avoid "unreachable return" warnings */
		exit(code);

	return 0;
}
/***
 * Parses the command line argument list *arg*.
 * The string *optstring* may contain the following elements:
 * individual characters, and characters followed by a colon.
 * A character followed by a single colon indicates that an
 * argument is to follow the option on the command line. For
 * example, * an option string `"x"` permits a **-x** option,
 * and an option string `"x:"` permits a **-x** option that
 * must take an argument. An option * string of `"x:yz"` permits
 * a **-x** option that takes an argument, and **-y** and **-z**
 * options, which do not.
 *
 * The function *fn* is run each time a new option of the argument
 * list is processed. It takes the parameters *opt*, *optarg*,
 * *optindex*, and *opterror*.
 * - *opt* is a string containing the option used. It is set
 * to the option the user specified on the command line.
 * If the user specifies an unknown option (one that is
 * not specified in *optstring*), the value of *opt* will
 * be set to nil. If the user specifies an option that
 * requires an argument, but does not specify its argument,
 * the value of *opt* will be the string `"*"` (a single
 * asterisk).
 * - *optarg*, is a string containing the option argument
 * (if applicable).
 * - *optindex* is an integer that contains the index of the
 * last command line argument processed. The last parameter,
 * *opterror*, is set in case of an option error (if *opt*
 * is nil or set to the string `"*"`), and is set to the
 * option that caused an error (in the case of *opt* being
 * nil, this will be the unknown option the user specified,
 * or in the case of *opt* being the string `"*"`, this
 * will be the option that required an argument).
 *
 * @function options
 * @usage
cl.options(arg, "abc:", function(opt, optarg, optindex, opterror)
	if opt == 'a' then
		print("-a was used")
	elseif opt == 'b' then
		print("-b was used")
	elseif opt == 'c' then
		print("-c was used, with argument " .. optarg)
	elseif opt == '*' then
		print("missing argument for -" .. opterror)
	elseif opt == nil then -- or ``if not opt``
		print("unknown option -" .. opterror)
	end
end)
 * @tparam        table arg The argument table to parse.
 * @tparam string optstring The option string, containing options that should be parsed.
 * @tparam      function fn The function to be run each time a new option is specified on the command line.
 */
static int
cl_options(lua_State *L)
{
	int argc, i;
	char ch, s[2];   /* opt character and string */
	char loptopt[2]; /* opterror, returned to Lua */
	char **argv;     /* parameter 1 (table), command line argument vector */
	char *optstring; /* parameter 2 (string), optstring passed to getopt */

	optstring = malloc(lua_rawlen(L, 2) * sizeof(char *));

	/* parameter type checking */
	luaL_checktype(L, 1, LUA_TTABLE);
	strbcpy(optstring, luaL_checkstring(L, 2), lua_rawlen(L, 2) * sizeof(char *));
	luaL_checktype(L, 3, LUA_TFUNCTION);

	argc = (int)lua_rawlen(L, 1) + 1; /* get argv length */
	argv = lua_newuserdatauv(L, (argc + 1) * sizeof(char *), 0);
	argv[argc] = NULL;

	for (i = 0; i < argc; i++) { /* for every argument */
		lua_pushinteger(L, i); /* push argv index */
		lua_gettable(L, 1);    /* push argv[i] */
		argv[i] = (char *)luaL_checkstring(L, -1);
	}

	if (optstring[0] == ':')
		return luaL_argerror(L, 2, "option string must not start with a colon (:)");

	strprepend(optstring, ":");

	/* getopt loop */
	while ((ch = getopt(argc, argv, optstring)) != -1) {
		/* construct string containing ch */
		s[0] = ch;
		s[1] = 0;
		lua_pushvalue(L, 3);

		/* first function parameter: opt */
		if (ch == '?') /* in case of unknown option */
			lua_pushnil(L);
		else if (ch == ':') /* in case of missing option argument */
			lua_pushliteral(L, "*");
		else /* otherwise just push the option character */
			lua_pushstring(L, s);

		/* second function parameter: optarg */
		lua_pushstring(L, optarg);
		/*  third function parameter: optindex */
		lua_pushinteger(L, optind);
		/* fourth function parameter: opterror
		 * (only non-nil on error) */
		if (optopt == '?') { /* error was not encountered */
			lua_pushnil(L);
		} else {
			loptopt[0] = optopt;
			loptopt[1] = 0;
			lua_pushstring(L, loptopt);
		}

		lua_pcall(L, 4, 0, 0); /* call Lua function */
	}

	free(optstring);
	return 0;
}

/* clang-format off */

static const luaL_Reg cllib[] = {
	{"mesg",      cl_mesg},
	{"error",     cl_error},
	{"panic",     cl_panic},
	{"options",   cl_options},
	{NULL, NULL}
};

int
luaopen_cl(lua_State *L)
{
	luaL_newlib(L, cllib);
	return 0;
}
