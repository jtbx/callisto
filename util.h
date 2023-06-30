#include <string.h>
#include <lua.h>

#define LFAIL_RET 2

#define streq(s1, s2) (strcmp((s1), (s2)) == 0)
#define newoverride(L, lib, libname)                \
	lua_getglobal(L, libname);                      \
	for (int i = 0; lib[i].name != NULL; i++) {     \
		lua_pushcfunction(L, lib[i].func);          \
		lua_setfield(L, -2, lib[i].name);           \
	}

int lfail(lua_State *, const char *);

#ifndef BSD
size_t strlcat(char *, const char *, size_t);
size_t strlcpy(char *, const char *, size_t);
#endif
void strprepend(char *, const char *);
