/*
 * Callisto, a featureful runtime for Lua 5.4.
 */

#ifndef _LCALLISTO_H_

#define _LCALLISTO_H_

#include <lua.h>

#define LUA_USE_POSIX

#define CALLISTO_VERSION_MAJOR		"0"
#define CALLISTO_VERSION_MINOR		"1"
#define CALLISTO_VERSION_RELEASE	"0"

#define CALLISTO_VERSION	"Callisto " CALLISTO_VERSION_MAJOR "." CALLISTO_VERSION_MINOR
#define CALLISTO_COPYRIGHT	CALLISTO_VERSION " (" LUA_RELEASE ")  Copyright (C) 1994-2022 Lua.org, PUC-Rio"

#define CALLISTO_CLLIBNAME   "cl"
#define CALLISTO_ENVLIBNAME  "environ"
#define CALLISTO_EXTLIBNAME  "_G" /* global table */
#define CALLISTO_FSYSLIBNAME "fs"
#define CALLISTO_JSONLIBNAME "json"
#define CALLISTO_MATHLIBNAME "math"
#define CALLISTO_OSLIBNAME   "os"
#define CALLISTO_PATHLIBNAME "path"
#define CALLISTO_PROCLIBNAME "process"
#define CALLISTO_SOCKLIBNAME "socket"

#define CALLISTO_ENVIRON "environ"

int luaopen_cl(lua_State *);
int luaopen_environ(lua_State *);
int luaopen_extra(lua_State *);
int luaopen_fs(lua_State *);
int luaopen_json(lua_State *);
/* luaopen_math and luaopen_os
 * are provided by Lua */
int luaopen_path(lua_State *);
int luaopen_process(lua_State *);
int luaopen_socket(lua_State *);

lua_State *callisto_newstate(void);
void callisto_openall(lua_State *);
void callisto_openlibs(lua_State *);
void callisto_setversion(lua_State *);

#endif
