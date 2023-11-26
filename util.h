#ifndef _UTIL_H_

#define _UTIL_H_

#include <string.h>
#include <lua.h>

int lfail(lua_State *);
int lfailm(lua_State *, const char *);

#ifndef BSD
size_t strlcat(char *, const char *, size_t);
size_t strlcpy(char *, const char *, size_t);
#endif
void strprepend(char *, const char *);

#endif
