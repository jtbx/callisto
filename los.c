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

static const luaL_Reg oslib[] = {
	{"hostname", os_hostname},
	{NULL, NULL}
};

int
luaopen_os(lua_State *L)
{
	newoverride(L, oslib, CALLISTO_OSLIBNAME);
	return 1;
}
