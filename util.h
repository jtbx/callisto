#ifndef _UTIL_H_

#define _UTIL_H_

#include <stddef.h>
#include <lua.h>

int lfail(lua_State *);
int lfailm(lua_State *, const char *);

size_t strbcat(char *, const char *, size_t);
size_t strbcpy(char *, const char *, size_t);
void strprepend(char *, const char *);

#endif
