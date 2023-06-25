/***
 * Networking and socket facilities, using LuaSocket.
 *
 * This module only provides the core, which
 * provides support for the low-level TCP and UDP
 * transport layers.
 *
 * @module socket
 */

#include <lua.h>
#include <lauxlib.h>

#include "lsocket.h"
#include "lcallisto.h"


int luaopen_socket_core(lua_State *L);

int
callistoopen_socket(lua_State *L)
{
	luaopen_socket_core(L);
	return 1;
}
