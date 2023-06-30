/*
 * Callisto, a featureful runtime for Lua 5.4.
 */

#ifndef _LCALLISTO_H_

#define _LCALLISTO_H_

#include <lua.h>

#define CALLISTO_VERSION_MAJOR		"0"
#define CALLISTO_VERSION_MINOR		"1"
#define CALLISTO_VERSION_RELEASE	"0"

#define CALLISTO_VERSION	"Callisto " CALLISTO_VERSION_MAJOR "." CALLISTO_VERSION_MINOR
#define CALLISTO_COPYRIGHT	CALLISTO_VERSION " (" LUA_RELEASE ")  Copyright (C) 1994-2022 Lua.org, PUC-Rio"

#define CALLISTO_CLLIBNAME   "cl"
#define CALLISTO_ENVLIBNAME  "environment"
#define CALLISTO_EXTLIBNAME  "_G" /* global table */
#define CALLISTO_FILELIBNAME "file"
#define CALLISTO_JSONLIBNAME "json"
#define CALLISTO_MATHLIBNAME "math"
#define CALLISTO_OSLIBNAME   "os"
#define CALLISTO_PROCLIBNAME "process"
#define CALLISTO_SOCKLIBNAME "socket"

int callistoopen_cl(lua_State *);
int callistoopen_environment(lua_State *);
int callistoopen_extra(lua_State *);
int callistoopen_file(lua_State *);
int callistoopen_json(lua_State *);
int callistoopen_math(lua_State *);
int callistoopen_os(lua_State *);
int callistoopen_process(lua_State *);
int callistoopen_socket(lua_State *);

lua_State *callisto_newstate(void);
void callisto_openlibs(lua_State *);
void callisto_setversion(lua_State *);

#endif
