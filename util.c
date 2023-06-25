/*
 * util.c
 *
 * Utility functions.
 * See util.h for a description
 * of these functions.
 */

#include <util.h>

#include <string.h>

#include <lua.h>
#include <lauxlib.h>

int lfail(lua_State *L, const char* mesg)
{
	luaL_pushfail(L);
	lua_pushstring(L, mesg);
	return LFAIL_RET;
}

void strprepend(char *s, const char *t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

void strslice(const char *s, char *buf, size_t start, size_t end)
{
	size_t j = 0;
	for (size_t i = start; i <= end; ++i) {
		buf[j++] = s[i];
	}
	buf[j] = 0;
}
