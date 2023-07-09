/***
 * Option parsing, and error formatting
 * for the command line.
 * @module cl
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


static void
fmesg(lua_State *L, FILE* f, int shift)
{
	int paramc, i;
	char *progname; /* argv[0] */

	paramc = lua_gettop(L); /* get parameter count */

	lua_geti(L, 1 + shift, 0); /* get index 0 of table at index 1 (argv) */
	if (lua_type(L, -1) != LUA_TSTRING) { /* if argv[0] is not a string... */
		luaL_argerror(L, 1, "must have a string in index 0)");
	}
	progname = (char *)lua_tostring(L, -1); /* set progname to argv[0] */

	/* format using string.format */
	lua_getglobal(L, "string");
	lua_getfield(L, -1, "format");

	for (i = 2 + shift; i <= paramc; i++) /* for every parameter */
		lua_pushvalue(L, i); /* push argument */

	lua_call(L, paramc - (1 + shift), 1); /* call string.format */

	fprintf(f, "%s: %s\n", basename(progname), lua_tostring(L, -1)); /* print */
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
	cl.error(arg, "could not open " .. err)
end
 * @tparam table  arg The command line argument table (this function only uses index *[0]*)
 * @tparam string message The message to print after the program's name. Supports *string.format*-style format specifiers.
 * @param  ... Any additional values specified by the format specifiers in the *message* parameter.
 */
static int
cl_error(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checkstring(L, 2);

	fmesg(L, stderr, 0);

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
 * @usage cl.mesg(arg, "message to stdout")
 * @tparam table  arg The command line argument table (this function only uses index *[0]*)
 * @tparam string message The message to print after the program's name. Supports *string.format*-style format specifiers.
 * @param  ... Any additional values specified by the format specifiers in the *message* parameter.
 */
static int
cl_mesg(lua_State *L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checkstring(L, 2);

	fmesg(L, stdout, 0);

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
	cl.panic(code, arg, "could not open " .. err)
	-- use the default exit code (1):
	cl.panic(arg, "could not open " .. err)
end
 * @tparam[opt] integer code The exit code to return to the OS.
 * @tparam table  arg The command line argument table (this function only uses index *[0]*)
 * @tparam string message The message to print after the program's name. Supports *string.format*-style format specifiers.
 * @param  ... Any additional values specified by the format specifiers in the *message* parameter.
 */
static int
cl_panic(lua_State *L)
{
	int code;

	if (lua_isinteger(L, 1)) { /* if an exit code was given... */
		code = lua_tointeger(L, 1); /* get code */
		luaL_checktype(L, 2, LUA_TTABLE);
		luaL_checkstring(L, 3);
		fmesg(L, stderr, 1);
	} else {
		code = 1;
		luaL_checktype(L, 1, LUA_TTABLE);
		luaL_checkstring(L, 2);
		fmesg(L, stderr, 0);
	}

	/* format using string.format */
	lua_getglobal(L, "os");
	lua_getfield(L, -1, "exit");

	/* push arguments to os.exit */
	lua_pushinteger(L, code);
	lua_pushboolean(L, 1);

	lua_call(L, 2, 0); /* call os.exit */

	return 0;
}
/***
 * Parses the command line argument list *arg*.
 * The string *optstring* may contain the
 * following elements: individual characters,
 * and characters followed by a colon.
 * A character followed by a single colon
 * indicates that an argument is to follow
 * the option on the command line. For example,
 * an option string `"x"` permits a **-x** option,
 * and an option string `"x:"` permits a **-x**
 * option that must take an argument. An option
 * string of `"x:yz"` permits a **-x** option that
 * takes an argument, and **-y** and **-z** options,
 * which do not.
 *
 * The function *fn* is run each time a new option
 * is processed. of the argument list. It takes the
 * parameters *opt*, *optarg*, *optindex*, and *opterror*.
 * *opt* is a string containing the option used. It is set
 * to the option the user specified on the command line.
 * If the user specifies an unknown option (one that is
 * not specified in *optstring*), the value of *opt* will
 * be set to nil. If the user specifies an option that
 * requires an argument, but does not specify its argument,
 * the value of *opt* will be the string `"*"` (a single
 * asterisk). The second parameter, *optarg*, is a string
 * containing the option argument (if applicable).
 * *optindex* is an integer that contains the index of the
 * last command line argument processed. The last parameter,
 * *opterror*, is set in case of an option error (if *opt*
 * is nil or set to the string `"*"`), and is set to the
 * option that caused an error (in the case of *opt* being
 * nil, this will be the unknown option the user specified,
 * or in the case of *opt* being the string `"*"`, this
 * will be the option that required an argument).
 *
 * @function parseopts
 * @usage
cl.parseopts(arg, "abc:", function(opt, optarg, optindex, opterror)
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
cl_parseopts(lua_State *L)
{
	int argc; /* command line argument count */
	int i;
	char ch, s[2];   /* opt character and string */
	char loptopt[2]; /* opterror, returned to Lua */
	char **argv;     /* parameter 1 (table), command line argument vector */
	char *optstring; /* parameter 2 (string), optstring passed to getopt */

	optstring = malloc(lua_rawlen(L, 2) * sizeof(char *));

	/* parameter type checking */
	luaL_checktype(L, 1, LUA_TTABLE);
	strlcpy(optstring, luaL_checkstring(L, 2), lua_rawlen(L, 2) * sizeof(char *));
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
		return luaL_error(L, "option string may not start with a colon (:)");

	strprepend(optstring, ":");

	/* getopt loop */
	while ((ch = getopt(argc, argv, optstring)) != -1) {
		/* construct string containing ch */
		s[0] = ch;
		s[1] = 0;
		lua_pushvalue(L, 3);

		/* first function parameter: opt */
		if (ch == '?')
			lua_pushnil(L); /* in case of unknown option */
		else if (ch == ':')
			lua_pushliteral(L, "*"); /* in case of missing option argument */
		else
			lua_pushstring(L, s);

		/* second function parameter: optarg */
		lua_pushstring(L, optarg);
		/*  third function parameter: optindex */
		lua_pushinteger(L, optind);
		/* fourth function parameter: opterror
		 * (only non-nil on error) */
		if (optopt == '?') /* error was not encountered */
			lua_pushnil(L);
		else {
			loptopt[0] = optopt;
			loptopt[1] = 0;
			lua_pushstring(L, loptopt);
		}

		lua_pcall(L, 4, 0, 0); /* call Lua function */
	}
	free(optstring);
	return 0;
}

static const luaL_Reg cllib[] = {
	{"mesg",      cl_mesg},
	{"error",     cl_error},
	{"panic",     cl_panic},
	{"parseopts", cl_parseopts},
	{NULL, NULL}
};

int
luaopen_cl(lua_State *L)
{
	luaL_newlib(L, cllib);
	return 0;
}
