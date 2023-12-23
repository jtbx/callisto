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
#ifdef __OpenBSD__
#	include <string.h>
#endif

#include <lua.h>
#include <lauxlib.h>

#include "callisto.h"
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
 * Returns the PID (process ID) of the given process.
 *
 * Returns nil if the process was not found.
 *
 * @function pidof
 * @usage process.pidof("init")
 * @tparam string process The name of the process to look up.
 */
static int
process_pidof(lua_State *L)
{
	const char *process; /* parameter 1 (string) */
	char       *command; /* pidof command buffer */
	char       *buffer;  /* pidof reading buffer */
	int     pexit;       /* pidof exit code */
	long    pid;         /* pid to return to Lua */
	size_t  pidmax;      /* length passed to getline */
	ssize_t ret;         /* getline return value */
	FILE   *p;           /* pidof stream */

	process = luaL_checkstring(L, 1);
	command = calloc(1, PROCESS_MAX * sizeof(char *));

	/* construct pidof command */
	strlcat(command, "pidof -s '", (size_t)PROCESS_MAX);
	strlcat(command, process,      (size_t)PROCESS_MAX);
	strlcat(command, "'",          (size_t)PROCESS_MAX);

	p = popen(command, "r");
	buffer = malloc(PID_MAX * sizeof(char *));
	pidmax = (size_t)PID_MAX;

	/* read line from pidof */
	ret = getline(&buffer, &pidmax, p);
	pexit = pclose(p);
	if (ret == -1 || pexit != 0) { /* did getline or pidof fail? */
		luaL_pushfail(L);
		return 1;
	}
	/* convert it to an integer and push */
	pid = strtol(buffer, NULL, 10);
	lua_pushinteger(L, pid);

	free(command);
	free(buffer);

	return 1;
}

static int
strtosig(const char *sig)
{
	int i;

	for (i = 1; i <= SIGC; i++) {
		if (strcmp(signals[i], sig) == 0)
			return i; /* valid signal found */
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
		return lfailm(L, "no such signal");

	return 1;
}

static int
sigsend(lua_State *L, pid_t pid, const char *sigstr)
{
	int ret, sig;

	sig = strtosig(sigstr);
	if (sig != -1) /* valid signal? */
		ret = kill(pid, sig);
	else
		return lfailm(L, "no such signal");

	if (ret == 0) { /* check for success */
		lua_pushboolean(L, 1);
		return 1;
	}

	return lfail(L);
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
