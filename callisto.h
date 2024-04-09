/*
 * Callisto - standalone scripting platform for Lua 5.4
 * Copyright (c) 2023-2024 Jeremy Baxter.
 */

/* clang-format off */

#ifndef _CALLISTO_H_
#define _CALLISTO_H_

#include <lua/lua.h>

#define CALLISTO_VERSION_MAJOR    "0"
#define CALLISTO_VERSION_MINOR    "1"
#define CALLISTO_VERSION_RELEASE  "0"

#define CALLISTO_VERSION \
	"Callisto " CALLISTO_VERSION_MAJOR "." CALLISTO_VERSION_MINOR
#define CALLISTO_COPYRIGHT \
	LUA_RELEASE "  Copyright (c) 1994-2023 Lua.org, PUC-Rio\n" \
	CALLISTO_VERSION "  Copyright (c) 2023-2024 Jeremy Baxter"

#define CALLISTO_CLLIBNAME   "cl"
#define CALLISTO_ENVLIBNAME  "environ"
#define CALLISTO_EXTLIBNAME  "_G" /* global table */
#define CALLISTO_FSYSLIBNAME "fs"
#define CALLISTO_JSONLIBNAME "json"
#define CALLISTO_PROCLIBNAME "process"

#define CALLISTO_ENVIRON "environ"

lua_State *callisto_newstate(void);
void callisto_openall(lua_State *);
void callisto_openlibs(lua_State *);
void callisto_setversion(lua_State *);

#endif
