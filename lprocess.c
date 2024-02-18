/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/***
 * Processes, signals, and signal handlers.
 *
 * @module process
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <lua.h>
#include <lauxlib.h>

#include "callisto.h"
#include "util.h"

#define PID_MAX 8 /* rounded to the nearest even number */
#define PROCESS_MAX 256

/* signals */
static const int signals[] = {
	SIGHUP,
	SIGINT,
	SIGQUIT,
	SIGILL,
	SIGTRAP,
	SIGABRT,
	SIGIOT,
	SIGFPE,
	SIGKILL,
	SIGBUS,
	SIGSEGV,
	SIGSYS,
	SIGPIPE,
	SIGALRM,
	SIGTERM,
	SIGURG,
	SIGSTOP,
	SIGTSTP,
	SIGCONT,
	SIGCHLD,
	SIGTTIN,
	SIGTTOU,
#ifdef SIGSTKFL
	SIGSTKFL,
#endif
	SIGIO,
	SIGXCPU,
	SIGXFSZ,
#ifdef SIGVTALR
	SIGVTALR,
#elif defined(SIGVTALRM)
	SIGVTALRM,
#endif
	SIGPROF,
#ifdef SIGWINC
	SIGWINC,
#elif defined(SIGWINCH)
	SIGWINCH,
#endif
#ifdef SIGINFO
	SIGINFO,
#endif
#ifdef SIGPOLL
	SIGPOLL,
#endif
#ifdef SIGPWR
	SIGPWR,
#endif
	SIGUSR1,
	SIGUSR2,
	-1 /* end */
};
static const char *sigstrs[] = {
	[SIGHUP]  = "SIGHUP",
	[SIGINT]  = "SIGINT",
	[SIGQUIT] = "SIGQUIT",
	[SIGILL]  = "SIGILL",
	[SIGTRAP] = "SIGTRAP",
	[SIGABRT] = "SIGABRT",
	[SIGIOT]  = "SIGIOT",
	[SIGFPE]  = "SIGFPE",
	[SIGKILL] = "SIGKILL",
	[SIGBUS]  = "SIGBUS",
	[SIGSEGV] = "SIGSEGV",
	[SIGSYS]  = "SIGSYS",
	[SIGPIPE] = "SIGPIPE",
	[SIGALRM] = "SIGALRM",
	[SIGTERM] = "SIGTERM",
	[SIGURG]  = "SIGURG",
	[SIGSTOP] = "SIGSTOP",
	[SIGTSTP] = "SIGTSTP",
	[SIGCONT] = "SIGCONT",
	[SIGCHLD] = "SIGCHLD",
	[SIGTTIN] = "SIGTTIN",
	[SIGTTOU] = "SIGTTOU",
#ifdef SIGSTKFL
	[SIGSTKFL] = "SIGSTKFL",
#endif
	[SIGIO]   = "SIGIO",
	[SIGXCPU] = "SIGXCPU",
	[SIGXFSZ] = "SIGXFSZ",
#ifdef SIGVTALR
	[SIGVTALR] = "SIGVTALR",
#elif defined(SIGVTALRM)
	[SIGVTALRM] = "SIGVTALRM",
#endif
	[SIGPROF] = "SIGPROF",
#ifdef SIGWINC
	[SIGWINC] = "SIGWINC",
#elif defined(SIGWINCH)
	[SIGWINCH] = "SIGWINCH",
#endif
#ifdef SIGINFO
	[SIGINFO] = "SIGINFO",
#endif
#ifdef SIGPOLL
	[SIGPOLL] = "SIGPOLL",
#endif
#ifdef SIGPWR
	[SIGPWR] = "SIGPWR",
#endif
	[SIGUSR1] = "SIGUSR1",
	[SIGUSR2] = "SIGUSR2"
};

/***
 * Returns the PID of the current process.
 *
 * @function pid
 * @usage local pid = process.pid()
 */
static int
process_pid(lua_State *L)
{
	lua_pushinteger(L, getpid());
	return 1;
}

/***
 * Returns the PID (process ID) of the given process,
 * or nil if the process could not be found.
 *
 * Depends on the nonportable userspace utility `pgrep`.
 * Can be found in the procps-ng package on Linux usually
 * included in most distributions, or as part of the base
 * system on OpenBSD, NetBSD and FreeBSD.
 *
 * @function pidof
 * @usage process.pidof("init")
 * @tparam string process The name of the process to look up.
 */
static int
process_pidof(lua_State *L)
{
	const char *process;        /* parameter 1 (string) */
	char  command[PROCESS_MAX]; /* pgrep command buffer */
	char *buffer;               /* pgrep reading buffer */
	int   pexit;                /* pgrep exit code */
	long  pid;                  /* pid to return to Lua */
	size_t  pidmax;             /* length passed to getline */
	ssize_t ret;                /* getline return value */
	FILE   *p;                  /* pgrep reading stream */

	process = luaL_checkstring(L, 1);

	/* construct pgrep command */
	memset(command, 0, PROCESS_MAX * sizeof(char));
	strbcat(command, "pgrep '",     PROCESS_MAX);
	strbcat(command, process,       PROCESS_MAX);
	strbcat(command, "' | sed 1q",  PROCESS_MAX);

	p = popen(command, "r");
	buffer = malloc(PID_MAX * sizeof(char *));
	pidmax = PID_MAX;

	/* read line from pgrep */
	ret = getline(&buffer, &pidmax, p);
	pexit = pclose(p);
	if (ret == -1 || pexit != 0) { /* did getline or pgrep fail? */
		lua_pushnil(L);
		return 1;
	}
	/* convert it to an integer and push */
	pid = strtol(buffer, NULL, 10);
	lua_pushinteger(L, pid);

	free(buffer);

	return 1;
}

#define REG_SIGC "callisto!process:sigc"

static int
initsignals(lua_State *L)
{
	int sigc;

	/* get the registry entry containing the sig count */
	lua_getfield(L, LUA_REGISTRYINDEX, REG_SIGC);
	/* if the registry entry isn't a number greater than 0... */
	if (!lua_isnoneornil(L, -1)) {
		if (lua_type(L, -1) != LUA_TNUMBER || lua_tointeger(L, -1) <= 0) {
			luaL_error(L, "registry index for signal count is invalid");
			return 0;
		}
	}

	sigc = 0;
	while (signals[sigc] != -1)
		sigc++;

	lua_pushinteger(L, sigc + 1);
	lua_setfield(L, LUA_REGISTRYINDEX, REG_SIGC);

	return 1;
}

static int
strtosig(const char *sig, int sigc)
{
	int i, j;

	j = 0;
	for (i = signals[j]; i < sigc; j++) {
		i = signals[j];
		if (strcasecmp(sigstrs[i], sig) == 0)
			return i; /* signal found */
	}
	return -1; /* invalid signal */
}

/***
 * Returns the given signal as an integer.
 *
 * This signal value is a platform-dependent value;
 * do not attempt to use it portably across different platforms.
 *
 * @function signum
 * @usage local sigkill = process.signum("SIGKILL")
 * @tparam string signal The signal to look up.
 */
static int
process_signum(lua_State *L)
{
	char *sigstr;
	int sig, sigc;

	sigstr = strdup(luaL_checkstring(L, 1));

	if (!initsignals(L))
		return 0;

	lua_getfield(L, LUA_REGISTRYINDEX, REG_SIGC);
	sigc = lua_tointeger(L, -1);
	sig = strtosig(sigstr, sigc);
	free(sigstr);

	if (sig != -1) { /* valid signal? */
		lua_pushinteger(L, sig); /* return signal */
		return 1;
	}

	return luaL_error(L, "no such signal");
}

static int
sigsend(lua_State *L, pid_t pid, const char *sigstr)
{
	int ret, sig, sigc;

	if (!initsignals(L))
		return 0;

	lua_getfield(L, LUA_REGISTRYINDEX, REG_SIGC);
	sigc = lua_tointeger(L, -1);

	sig = strtosig(sigstr, sigc);
	if (sig != -1) /* valid signal? */
		ret = kill(pid, sig);
	else
		return luaL_error(L, "no such signal");

	if (ret == 0) { /* check for success */
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
}

#undef REG_SIGC

/***
 * Sends the given signal to the process with the given PID.
 *
 * The *signal* parameter is a string containing the name
 * of the desired signal to send (e.g. SIGKILL).
 *
 * @function send
 * @usage
local pid = process.pid("sh")
process.send(pid, "SIGTERM")
 * @tparam integer pid The PID of the process.
 * @tparam string signal The signal to send.
 */
static int
process_send(lua_State *L)
{
    pid_t pid;       /* parameter 1 (integer) */
	const char *sig; /* parameter 2 (string)  */

	pid = luaL_checkinteger(L, 1);
	sig = luaL_checkstring(L, 2);

	return sigsend(L, pid, sig);
}

/***
 * Kills the process with the given PID.
 *
 * Equivalent to `process.send(pid, "SIGKILL")`.
 *
 * @function kill
 * @tparam integer pid The PID of the process.
 */
static int
process_kill(lua_State *L)
{
    pid_t pid;       /* parameter 1 (integer) */

	pid = luaL_checkinteger(L, 1);

	return sigsend(L, pid, "SIGKILL");
}
/***
 * Terminates the process with the given PID.
 *
 * Equivalent to `process.send(pid, "SIGTERM")`.
 *
 * @function terminate
 * @tparam integer pid The PID of the process.
 */
static int
process_terminate(lua_State *L)
{
    pid_t pid;       /* parameter 1 (integer) */

	pid = luaL_checkinteger(L, 1);

	return sigsend(L, pid, "SIGTERM");
}

static const luaL_Reg proclib[] = {
	{"kill",      process_kill},
	{"pid",       process_pid},
	{"pidof",     process_pidof},
	{"send",      process_send},
	{"signum",    process_signum},
	{"terminate", process_terminate},
	{NULL, NULL}
};

int
luaopen_process(lua_State *L)
{
	luaL_newlib(L, proclib);
	return 0;
}
