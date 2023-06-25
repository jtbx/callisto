/***
 * Operating system related facilities.
 * @module os
 */

#ifdef __linux__
#	include <bits/local_lim.h>
#else
/* assume OpenBSD/NetBSD */
#	include <sys/syslimits.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>

#include "los.h"
#include "lcallisto.h"
#include "util.h"


/***
 * Returns the system hostname.
 *
 * @function hostname
 * @usage local hostname = os.hostname()
 */
static int
os_hostname(lua_State *L)
{
	char *buffer;

	buffer = malloc(HOST_NAME_MAX * sizeof(char *));

	gethostname(buffer, HOST_NAME_MAX); /* get hostname */
	lua_pushstring(L, buffer);
	free(buffer);

    return 1;
}
/***
 * Waits the specified amount of seconds.
 *
 * @function sleep
 * @usage
local minutes = 5
os.sleep(minutes * 60) -- 5 minutes
 * @tparam number seconds The amount of seconds to wait.
 */
static int
os_sleep(lua_State *L)
{
	/* Implementation from luasocket */
    double n;
    struct timespec t, r;

	n = luaL_checknumber(L, 1);

    if (n < 0.0)
		n = 0.0;
    if (n > INT_MAX)
		n = INT_MAX;

    t.tv_sec = (int)n;
    n -= t.tv_sec;
    t.tv_nsec = (int)(n * 1000000000);

    if (t.tv_nsec >= 1000000000)
		t.tv_nsec = 999999999;

    while (nanosleep(&t, &r) != 0) {
        t.tv_sec = r.tv_sec;
        t.tv_nsec = r.tv_nsec;
    }
    return 0;
}

static const luaL_Reg oslib[] = {
	{"hostname", os_hostname},
	{"sleep",    os_sleep},
	{NULL, NULL}
};

int
callistoopen_os(lua_State *L)
{
	newoverride(L, oslib, CALLISTO_OSLIBNAME);
	return 1;
}
