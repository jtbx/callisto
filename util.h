#include <lua.h>

#define LFAIL_RET 2

#define newoverride(L, lib, libname)                \
	lua_getglobal(L, libname);                      \
	for (int i = 0; lib[i].name != NULL; i++) {     \
		lua_pushcfunction(L, lib[i].func);          \
		lua_setfield(L, -2, lib[i].name);           \
	}

/*
 * Pushes the fail value and the string mesg
 * onto the stack, then returns 2.
 */
int lfail(lua_State *L, const char* mesg);
/*
 * Prepends t onto s.
 * Assumes s has enough space allocated
 * for the combined string.
 */
void strprepend(char* s, const char* t);

/*
 * Slices the string s into buf.
 * The sliced string has the range [start .. end].
 */
void strslice(const char *s, char *buf, size_t start, size_t end);
