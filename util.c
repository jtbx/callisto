/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/*
 * util.c
 *
 * Utility functions.
 */

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <locale.h>
#include <math.h>
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

#define MAX_FORMAT  32
#define MAX_ITEMF   (110 + l_floatatt(MAX_10_EXP))
#define MAX_ITEM    120
#define L_ESC       '%'
#define L_FMTFLAGSF "-+#0 "
#define L_FMTFLAGSX "-#0"
#define L_FMTFLAGSI "-+0 "
#define L_FMTFLAGSU "-0"
#define L_FMTFLAGSC "-"
#define uchar(c)    ((unsigned char)(c))

static void addlenmod(char *, const char *);
static void addliteral(lua_State *, luaL_Buffer *, int);
static void addquoted(luaL_Buffer *, const char *, size_t);
static void checkformat(lua_State *, const char *, const char *, int);
static const char *get2digits(const char *);
static const char *getformat(lua_State *, const char *, char *);
static int quotefloat(lua_State *, char *, lua_Number);

int
lstrfmt(lua_State *L)
{
	luaL_Buffer b;
	const char *flags;
	const char *strfrmt;
	const char *strfrmt_end;
	size_t sfl;
	int arg;
	int top;

	top = lua_gettop(L);
	arg = 1;
	strfrmt = luaL_checklstring(L, arg, &sfl);
	strfrmt_end = strfrmt + sfl;

	luaL_buffinit(L, &b);

	while (strfrmt < strfrmt_end) {
		if (*strfrmt != L_ESC)
			luaL_addchar(&b, *strfrmt++);
		else if (*++strfrmt == L_ESC)
			luaL_addchar(&b, *strfrmt++); /* %% */
		else {                            /* format item */
			char form[MAX_FORMAT];        /* to store the format ('%...') */
			int maxitem = MAX_ITEM;       /* maximum length for the result */
			char *buff = luaL_prepbuffsize(&b, maxitem); /* to put result */
			int nb = 0; /* number of bytes in result */
			if (++arg > top)
				return luaL_argerror(L, arg, "no value");
			strfrmt = getformat(L, strfrmt, form);
			switch (*strfrmt++) {
			case 'c': {
				checkformat(L, form, L_FMTFLAGSC, 0);
				nb = l_sprintf(buff, maxitem, form,
					(int)luaL_checkinteger(L, arg));
				break;
			}
			case 'd':
			case 'i':
				flags = L_FMTFLAGSI;
				goto intcase;
			case 'u':
				flags = L_FMTFLAGSU;
				goto intcase;
			case 'o':
			case 'x':
			case 'X':
				flags = L_FMTFLAGSX;
intcase: {
	lua_Integer n = luaL_checkinteger(L, arg);
	checkformat(L, form, flags, 1);
	addlenmod(form, LUA_INTEGER_FRMLEN);
	nb = l_sprintf(buff, maxitem, form, (LUAI_UACINT)n);
	break;
}
			case 'a':
			case 'A':
				checkformat(L, form, L_FMTFLAGSF, 1);
				addlenmod(form, LUA_NUMBER_FRMLEN);
				nb = lua_number2strx(L, buff, maxitem, form,
					luaL_checknumber(L, arg));
				break;
			case 'f':
				maxitem = MAX_ITEMF; /* extra space for '%f' */
				buff = luaL_prepbuffsize(&b, maxitem);
				/* FALLTHROUGH */
			case 'e':
			case 'E':
			case 'g':
			case 'G': {
				lua_Number n = luaL_checknumber(L, arg);
				checkformat(L, form, L_FMTFLAGSF, 1);
				addlenmod(form, LUA_NUMBER_FRMLEN);
				nb = l_sprintf(buff, maxitem, form, (LUAI_UACNUMBER)n);
				break;
			}
			case 'p': {
				const void *p = lua_topointer(L, arg);
				checkformat(L, form, L_FMTFLAGSC, 0);
				if (p == NULL) {  /* avoid calling 'printf' with argument NULL */
					p = "(null)"; /* result */
					form[strlen(form) - 1] = 's'; /* format it as a string */
				}
				nb = l_sprintf(buff, maxitem, form, p);
				break;
			}
			case 'q': {
				if (form[2] != '\0') /* modifiers? */
					return luaL_error(L, "specifier '%%q' cannot have modifiers");
				addliteral(L, &b, arg);
				break;
			}
			case 's': {
				size_t l;
				const char *s = luaL_tolstring(L, arg, &l);
				if (form[2] == '\0')   /* no modifiers? */
					luaL_addvalue(&b); /* keep entire string */
				else {
					luaL_argcheck(L, l == strlen(s), arg,
						"string contains zeros");
					checkformat(L, form, L_FMTFLAGSC, 1);
					if (strchr(form, '.') == NULL && l >= 100) {
						/* no precision and string is too long to be formatted */
						luaL_addvalue(&b); /* keep entire string */
					} else {               /* format the string into 'buff' */
						nb = l_sprintf(buff, maxitem, form, s);
						lua_pop(L, 1); /* remove result from 'luaL_tolstring' */
					}
				}
				break;
			}
			default: { /* also treat cases 'pnLlh' */
				return luaL_error(L, "invalid conversion '%s' to 'format'", form);
			}
			}
			lua_assert(nb < maxitem);
			luaL_addsize(&b, nb);
		}
	}

	luaL_pushresult(&b);
	return 1;
}

static void
addlenmod(char *form, const char *lenmod)
{
	size_t l, lm;
	char spec;

	l = strlen(form);
	lm = strlen(lenmod);
	spec = form[l - 1];
	strcpy(form + l - 1, lenmod);
	form[l + lm - 1] = spec;
	form[l + lm] = '\0';
}

static void
addliteral(lua_State *L, luaL_Buffer *b, int arg)
{
	switch (lua_type(L, arg)) {
	case LUA_TSTRING: {
		size_t len;
		const char *s = lua_tolstring(L, arg, &len);
		addquoted(b, s, len);
		break;
	}
	case LUA_TNUMBER: {
		char *buff = luaL_prepbuffsize(b, MAX_ITEM);
		int nb;
		if (!lua_isinteger(L, arg)) /* float? */
			nb = quotefloat(L, buff, lua_tonumber(L, arg));
		else { /* integers */
			lua_Integer n = lua_tointeger(L, arg);
			const char *format = (n == LUA_MININTEGER) /* corner case? */
				? "0x%" LUA_INTEGER_FRMLEN "x"         /* use hex */
				: LUA_INTEGER_FMT; /* else use default format */
			nb = l_sprintf(buff, MAX_ITEM, format, (LUAI_UACINT)n);
		}
		luaL_addsize(b, nb);
		break;
	}
	case LUA_TNIL:
	case LUA_TBOOLEAN: {
		luaL_tolstring(L, arg, NULL);
		luaL_addvalue(b);
		break;
	}
	default: {
		luaL_argerror(L, arg, "value has no literal form");
	}
	}
}

static void
addquoted(luaL_Buffer *b, const char *s, size_t len)
{
	luaL_addchar(b, '"');
	while (len--) {
		if (*s == '"' || *s == '\\' || *s == '\n') {
			luaL_addchar(b, '\\');
			luaL_addchar(b, *s);
		} else if (iscntrl(uchar(*s))) {
			char buff[10];
			if (!isdigit(uchar(*(s + 1))))
				l_sprintf(buff, sizeof(buff), "\\%d", (int)uchar(*s));
			else
				l_sprintf(buff, sizeof(buff), "\\%03d", (int)uchar(*s));
			luaL_addstring(b, buff);
		} else
			luaL_addchar(b, *s);
		s++;
	}
	luaL_addchar(b, '"');
}

static void
checkformat(lua_State *L, const char *form, const char *flags, int precision)
{
	const char *spec;

	spec = form + 1;             /* skip '%' */
	spec += strspn(spec, flags); /* skip flags */
	if (*spec != '0') {          /* a width cannot start with '0' */
		spec = get2digits(spec); /* skip width */
		if (*spec == '.' && precision) {
			spec++;
			spec = get2digits(spec); /* skip precision */
		}
	}
	if (!isalpha(uchar(*spec))) /* did not go to the end? */
		luaL_error(L, "invalid conversion specification: '%s'", form);
}

static const char *
get2digits(const char *s)
{
	if (isdigit(uchar(*s))) {
		s++;
		if (isdigit(uchar(*s)))
			s++; /* (2 digits at most) */
	}
	return s;
}

static const char *
getformat(lua_State *L, const char *strfrmt, char *form)
{
	size_t len;

	/* spans flags, width, and precision ('0' is included as a flag) */
	len = strspn(strfrmt, L_FMTFLAGSF "123456789.");
	len++; /* adds following character (should be the specifier) */
	/* still needs space for '%', '\0', plus a length modifier */
	if (len >= MAX_FORMAT - 10)
		luaL_error(L, "invalid format (too long)");
	*(form++) = '%';
	memcpy(form, strfrmt, len * sizeof(char));
	*(form + len) = '\0';
	return strfrmt + len - 1;
}

static int
quotefloat(lua_State *L, char *buff, lua_Number n)
{
	const char *s; /* for the fixed representations */

	if (n == (lua_Number)HUGE_VAL) /* inf? */
		s = "1e9999";
	else if (n == -(lua_Number)HUGE_VAL) /* -inf? */
		s = "-1e9999";
	else if (n != n) /* NaN? */
		s = "(0/0)";
	else { /* format number as hexadecimal */
		int nb = lua_number2strx(L, buff, MAX_ITEM, "%" LUA_NUMBER_FRMLEN "a", n);
		/* ensures that 'buff' string uses a dot as the radix character */
		if (memchr(buff, '.', nb) == NULL) {      /* no dot? */
			char point = lua_getlocaledecpoint(); /* try locale point */
			char *ppoint = (char *)memchr(buff, point, nb);
			if (ppoint)
				*ppoint = '.'; /* change it to a dot */
		}
		return nb;
	}
	/* for the fixed representations */
	return l_sprintf(buff, MAX_ITEM, "%s", s);
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
