/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/*
 * util.c
 *
 * Utility functions.
 */

#include <errno.h>
#include <string.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include "util.h"

int
lfail(lua_State *L)
{
	char strerrbuf[256];

	luaL_pushfail(L);
	strerror_r(errno, strerrbuf, 256);
	lua_pushstring(L, strerrbuf);
	lua_pushinteger(L, errno);
	return 3;
}

int
lfailm(lua_State *L, const char *mesg)
{
	luaL_pushfail(L);
	lua_pushstring(L, mesg);
	return 2;
}

/*
 * strbcat and strbcpy are from OpenBSD source files
 * lib/libc/string/strlcat.c and
 * lib/libc/string/strlcpy.c respectively
 */

/*
 * Appends src to string dst of size dsize (unlike strncat, dsize is the
 * full size of dst, not space left).  At most dsize-1 characters
 * will be copied.  Always NUL terminates (unless dsize <= strlen(dst)).
 * Returns strlen(src) + MIN(dsize, strlen(initial dst)).
 * If retval >= dsize, truncation occurred.
 */
size_t
strbcat(char *dst, const char *src, size_t dsize)
{
	const char *odst = dst;
	const char *osrc = src;
	size_t n = dsize;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end. */
	while (n-- != 0 && *dst != '\0')
		dst++;
	dlen = dst - odst;
	n = dsize - dlen;

	if (n-- == 0)
		return (dlen + strlen(src));
	while (*src != '\0') {
		if (n != 0) {
			*dst++ = *src;
			n--;
		}
		src++;
	}
	*dst = '\0';

	return (dlen + (src - osrc)); /* count does not include NUL */
}

/*
 * Copy string src to buffer dst of size dsize.  At most dsize-1
 * chars will be copied.  Always NUL terminates (unless dsize == 0).
 * Returns strlen(src); if retval >= dsize, truncation occurred.
 */
size_t
strbcpy(char *dst, const char *src, size_t dsize)
{
	const char *osrc = src;
	size_t nleft = dsize;

	/* Copy as many bytes as will fit. */
	if (nleft != 0) {
		while (--nleft != 0) {
			if ((*dst++ = *src++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src. */
	if (nleft == 0) {
		if (dsize != 0)
			*dst = '\0'; /* NUL-terminate dst */
		while (*src++)
			;
	}

	return (src - osrc - 1); /* count does not include NUL */
}

/*
 * Prepends t to s.
 */
void
strprepend(char *s, const char *t)
{
	size_t len = strlen(t);
	memmove(s + len, s, strlen(s) + 1);
	memcpy(s, t, len);
}
