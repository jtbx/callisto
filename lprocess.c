/***
 * Processes, signals, and
 * signal handlers.
 * @module process
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#ifdef BSD
#	include <string.h>
#endif

#include <lua.h>
#include <lauxlib.h>

#include "lcallisto.h"
#include "util.h"

#define PID_MAX 8 /* rounded to the nearest even number */
#define PROCESS_MAX 256


/* signals and signal count */
#define SIGC 36
static const char *signals[] = {
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
#elif defined (SIGWINCH)
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
	lua_pushinteger(L, getpid()); /* push current process pid */
	return 1;
}
/***
 * Returns the ID of the given process.
 *
 * Returns nil if the process was not found.
 *
 * @function pidof
 * @usage process.pidof("init")
 * @tparam string process The process to look up.
 */
static int
process_pidof(lua_State *L)
{
	const char *process; /* parameter 1 (string) */
	char *command;       /* pidof command buffer */
	char *buffer;        /* pidof reading buffer */
	long pid;            /* pid to return to Lua */
	size_t pidmax;       /* length passed to getline */
	ssize_t ret;         /* return value of getline */
	FILE *p;

	if (lua_isnoneornil(L, 1)) { /* check if first parameter wasn't given */
		lua_pushinteger(L, getpid()); /* push current process pid */
		return 1;
	}

	process = luaL_checkstring(L, 1);
	command = calloc(1, PROCESS_MAX * sizeof(char *));

	/* construct pidof command */
	strlcat(command, "pidof -s '", (size_t)PROCESS_MAX);
	strlcat(command, process,      (size_t)PROCESS_MAX);
	strlcat(command, "'",          (size_t)PROCESS_MAX);

	p = popen(command, "r");
	buffer = malloc(PID_MAX * sizeof(char *));
	pidmax = (size_t)PID_MAX;
	ret = getline(&buffer, &pidmax, p); /* get line from pidof */
	if (ret == -1 || pclose(p) != 0) { /* did getline or pidof fail? */
		luaL_pushfail(L);
		return 1;
	}
	pid = strtol(buffer, NULL, 10); /* convert it to an integer */
	lua_pushinteger(L, pid); /* push pid */
	free(command);
	free(buffer);
	return 1;
}

static int
strtosig(const char *sig)
{
	int i;

	for (i = 1; i <= SIGC; i++) {
		if (streq(signals[i], sig)) {
			return i; /* valid signal found */
		}
	}
	return -1; /* invalid signal */
}

/***
 * Returns the given signal as an integer.
 * This function may return different values
 * across different operating systems, as
 * signal constants vary across different Unixes.
 *
 * @function signum
 * @usage local sigkill = process.signum("SIGKILL")
 * @tparam string signal The signal to look up.
 */
static int
process_signum(lua_State *L)
{
	int sig;

	if ((sig = strtosig(luaL_checkstring(L, 1))) != -1) /* valid signal? */
		lua_pushinteger(L, sig); /* return signal */
	else
		return lfail(L, "no such signal");

	return 1;
}

static int
sigsend(lua_State *L, pid_t pid, const char *sigstr)
{
	int ret, sig;

	sig = strtosig(sigstr);
	if (sig != -1) {
		ret = kill(pid, sig);
	} else {
		lfail(L, "no such signal");
		return 0;
	}

	if (ret == 0) { /* check for success */
		lua_pushboolean(L, 1);
		return 1;
	}

	switch (errno) {
		case ESRCH:
			lfail(L, "no such process");
			break;
		case EPERM:
			lfail(L, "permission denied");
			break;
	}
	return 0;
}

/***
 * Sends the given signal to the
 * process with the given PID.
 *
 * The *signal* parameter is a string
 * containing the name of the desired
 * signal to send..
 *
 * @function send
 * @usage
local pid = process.pid("sh")
process.send(pid, process.SIGTERM)
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

	if (sigsend(L, pid, sig))
		return 1;
	else
		return LFAIL_RET;
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

	if (sigsend(L, pid, "SIGKILL"))
		return 1;
	else
		return LFAIL_RET;
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

	if (sigsend(L, pid, "SIGTERM"))
		return 1;
	else
		return LFAIL_RET;
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
