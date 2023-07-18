# Callisto

A featureful runtime for Lua 5.4, written in C99 using POSIX APIs.

Callisto is an extension to Lua that adds commonly-needed functions
and features to the language, and includes a file library to manage
and manipulate files, a process library to find active processes and
manipulate signals, a socket and networking library using LuaSocket,
and a JSON manipulation library *among many more*.

Before I made Callisto, I usually had to rely on three libraries:
luaposix for basic file manipulation and other routines, lua-cjson
for JSON parsing support and LuaSocket for networking.

First and foremost, Callisto tries to satisfy these requirements:
 - an all-in-one zero-dependencies library for Lua that includes
   most features people would need, out of the box
 - a library that works and integrates well with Lua and its
   standard library, and is easy to use

Currently Callisto only works on POSIX operating systems such as
Linux and the BSDs (mainly because I don't have a machine running
Windows).

## Dependencies

To build Callisto, you'll need nothing but a C compiler.
The default C compiler is *cc* which is usually a symbolic link
to your system's default C compiler. This should be gcc on Linux,
and clang on most of the BSDs. If *cc* doesn't exist on your system,
change the `CC` variable in the Makefile to your desired C compiler.

**Callisto has zero runtime dependencies**, unless you built it with
support for GNU libreadline. Lua 5.4 is statically linked in.

## Installation

Callisto is distributed as source-only, but don't worry, it's
not hard to compile.

First, get the source code using one of the tarballs found in
the [Releases](https://github.com/jtbx/callisto/releases) page.
Untar it then run `make` to compile Callisto. The compiled
executable will be named `csto`.

To install `csto` (the Callisto standalone executable) and
`libcallisto.so` (the Callisto shared library), run `make install`
as the root user in the source code directory.
