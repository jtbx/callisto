/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stddef.h>

#include <lua/lua.h>

int lfail(lua_State *);
int lfailm(lua_State *, const char *);
int lstrfmt(lua_State *);

size_t strbcat(char *, const char *, size_t);
size_t strbcpy(char *, const char *, size_t);
void strprepend(char *, const char *);

#endif
