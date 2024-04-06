/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/***
 * Processes, signals, and signal handlers.
 *
 * @module process
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "util.h"

#define PID_MAX     8 /* rounded to the nearest even number */
#define PROCESS_MAX 256

static const int sigc = 22;

/* clang-format off */

static const int signals[] = {
	SIGABRT,
	SIGALRM,
	SIGBUS,
	SIGCHLD,
	SIGCONT,
	SIGFPE,
	SIGHUP,
	SIGILL,
	SIGINT,
	SIGKILL,
	SIGPIPE,
	SIGQUIT,
	SIGSEGV,
	SIGSTOP,
	SIGTERM,
	SIGTSTP,
	SIGTTIN,
	SIGTTOU,
	SIGUSR1,
	SIGUSR2,
	SIGTRAP,
	SIGURG
};

static const char *sigstrs[] = {
	[SIGABRT] = "SIGABRT",
	[SIGALRM] = "SIGALRM",
	[SIGBUS] = "SIGBUS",
	[SIGCHLD] = "SIGCHLD",
	[SIGCONT] = "SIGCONT",
	[SIGFPE] = "SIGFPE",
	[SIGHUP] = "SIGHUP",
	[SIGILL] = "SIGILL",
	[SIGINT] = "SIGINT",
	[SIGKILL] = "SIGKILL",
	[SIGPIPE] = "SIGPIPE",
	[SIGQUIT] = "SIGQUIT",
	[SIGSEGV] = "SIGSEGV",
	[SIGSTOP] = "SIGSTOP",
	[SIGTERM] = "SIGTERM",
	[SIGTSTP] = "SIGTSTP",
	[SIGTTIN] = "SIGTTIN",
	[SIGTTOU] = "SIGTTOU",
	[SIGUSR1] = "SIGUSR1",
	[SIGUSR2] = "SIGUSR2",
	[SIGTRAP] = "SIGTRAP",
	[SIGURG] = "SIGURG"
};

/* clang-format on */

static int
process_pid(lua_State *L)
{
	lua_pushinteger(L, getpid());
	return 1;
}

static int
process_pidof(lua_State *L)
{
	const char *process;       /* parameter 1 (string) */
	char command[PROCESS_MAX]; /* pgrep command buffer */
	char *buffer;              /* pgrep reading buffer */
	int pexit;                 /* pgrep exit code */
	long pid;                  /* pid to return to Lua */
	size_t pidmax;             /* length passed to getline */
	ssize_t ret;               /* getline return value */
	FILE *p;                   /* pgrep reading stream */

	process = luaL_checkstring(L, 1);

	/* construct pgrep command */
	memset(command, 0, PROCESS_MAX * sizeof(char));
	strbcat(command, "pgrep '", PROCESS_MAX);
	strbcat(command, process, PROCESS_MAX);
	strbcat(command, "' | sed 1q", PROCESS_MAX);

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

static int
strtosig(const char *sig)
{
	int i, j;

	j = 0;
	for (i = signals[j]; j < sigc; j++) {
		i = signals[j];
		if (strcasecmp(sigstrs[i], sig) == 0)
			return i; /* signal found */
	}
	return -1; /* invalid signal */
}

static int
process_signum(lua_State *L)
{
	const char *sigstr;
	int sig;

	sigstr = luaL_checkstring(L, 1);
	sig = strtosig(sigstr);

	if (sig != -1) {             /* valid signal? */
		lua_pushinteger(L, sig); /* return signal */
		return 1;
	}

	return luaL_error(L, "no such signal");
}

static int
sigsend(lua_State *L, pid_t pid, const char *sigstr)
{
	int ret, sig;

	sig = strtosig(sigstr);
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

static int
process_send(lua_State *L)
{
	pid_t pid;       /* parameter 1 (integer) */
	const char *sig; /* parameter 2 (string)  */

	pid = luaL_checkinteger(L, 1);
	sig = luaL_checkstring(L, 2);

	return sigsend(L, pid, sig);
}

static int
process_kill(lua_State *L)
{
	pid_t pid; /* parameter 1 (integer) */

	pid = luaL_checkinteger(L, 1);

	return sigsend(L, pid, "SIGKILL");
}

static int
process_terminate(lua_State *L)
{
	pid_t pid; /* parameter 1 (integer) */

	pid = luaL_checkinteger(L, 1);

	return sigsend(L, pid, "SIGTERM");
}

/* clang-format off */

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
