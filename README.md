# Callisto

A featureful extension runtime for Lua 5.4, using POSIX APIs.

Callisto is an extension to Lua that adds commonly-needed functions
and features to the language, and includes a file system library to
manage and manipulate files, a process library to find active processes
and manipulate signals, a socket and networking library using LuaSocket,
and a JSON manipulation library *among many more*.

It is a standalone program designed for people using Lua as a
general scripting language, instead of using it embedded into
another application.

Before I made Callisto, I usually had to rely on three libraries:
luaposix for basic file system manipulation and other routines,
lua-cjson for JSON parsing support and LuaSocket for networking.

luaposix provides most of the necessary functions, but is
generally aimed towards people who already know how to use
the POSIX APIs in C.

First and foremost, Callisto tries to be:
 - an all-in-one zero-dependencies library for Lua that includes
   most features people would need, out of the box
 - a library that works and integrates well with Lua and its
   standard library, and is easy to use for those who have no
   experience with C

Callisto relies on APIs specified in the POSIX specification;
therefore it does not support operating systems that do not
implement these APIs (like Microsoft Windows), only ones that
do (like Linux and the BSDs).

## Dependencies

To build Callisto, you'll need nothing but a C compiler.
The default C compiler is *cc* which is usually a symbolic link
to your system's default C compiler. This should be gcc on Linux,
and clang on most of the BSDs. If *cc* doesn't exist on your system,
override it by adding `CC=cc` to `make`'s command like (replace `cc` with
the name of or the path to your C compiler)

**Callisto has zero runtime dependencies**, unless you built it with
support for GNU libreadline. Lua 5.4 is statically linked in.

## Installation

Callisto is distributed as source-only, but it's not hard to compile.

First, get the source code using one of the tarballs found in
the [Releases](https://github.com/jtbx/callisto/releases) page.
Untar it then enter the directory with Callisto's source code.

After that, run
```
make
```
to compile Callisto and all its dependencies.

To install it, run `make install` as the root user in the source code directory
to install Callisto and its shared library.

## Usage

The standalone Callisto interpreter is called `csto`. Running it
will start a REPL so you can execute chunks of code interactively.

csto works just like the standalone Lua 5.4 interpreter. To execute
a file, run `csto <file>` where *\<file\>* is the name of the file
you want to run. Alternatively, you can put `#!/usr/bin/env csto` at
the top of your script, run `chmod +x` on it, and then you can run the
script as if it was a standalone executable, for example `./yourscript.lua`.
