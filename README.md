## callisto - standalone scripting platform for Lua 5.4

Callisto extends Lua 5.4's standard library by adding new libraries and
facilities to the language. It includes a file system library to manage
and manipulate files, a process library to find active processes and
manipulate signals, and a JSON manipulation library (lua-cjson) *among
many more*.

It is a standalone interpreter designed for people using Lua as a
general scripting language, instead of using it embedded into another
application (what Lua was designed for).

Before I made Callisto, I had to rely on luaposix for basic file system
manipulation and occasionally luasocket for HTTP plus lua-cjson for JSON
parsing.

luaposix provides most of the necessary functions, but is generally
aimed towards people who already know how to use the POSIX APIs in C.

First and foremost, Callisto tries to be:
 - an all-in-one zero-dependencies library for Lua that includes
   most features people would need, out of the box
 - a library that works and integrates well with Lua and its
   standard library, and is easy to use for those who have no
   experience with C

Callisto relies on APIs specified in the POSIX specification;
therefore it does not support operating systems that do not
implement these APIs (like Microsoft Windows), only ones that
do (like Linux, macOS, and the BSDs).

## Dependencies

To build Callisto, you'll need nothing but a C compiler and `ar`.
The default C compiler is *cc* which is usually a symbolic link
to your system's default C compiler. This should be gcc on Linux,
and clang on most of the BSDs. If *cc* doesn't exist on your system,
override it by adding `CC=compiler` to make's command like
(replace `compiler` with the name or path to your C compiler)

### Portability

**Callisto has zero runtime dependencies**, unless you built it with
support for GNU libreadline.* Lua 5.4 is statically linked in.
This means that the same binary will likely work across different Linux
distributions/versions. The only strictly required library is libc
which is available on all systems.

*libreadline support can be enabled at build time, but is disabled by
default. To force building with libreadline support, pass the
`-wreadline` flag to the configure script.

## Installation

Callisto is distributed as source-only, but it's not hard to compile.

First, get the source code using one of the tarballs found in
the [Releases](https://github.com/jtbx/callisto/releases) page.
Untar it then enter the directory with Callisto's source code.

After that, run

    ./configure
    make

to compile Callisto and all its dependencies.

To install it, run `make install` as the root user in the source code directory
to install Callisto and its shared library.

### Arch Linux

Users of Arch Linux can install the AUR package:

https://aur.archlinux.org/packages/callisto-git

### Nix

If you use Nix, you can use the flake:

    nix profile install github:jtbx/callisto

## Usage

The standalone Callisto interpreter is called `csto`. Running it
will start a REPL so you can execute chunks of code interactively.

csto works just like the standalone Lua 5.4 interpreter. To execute
a file, run `csto file` where *file* is the name of the file containing
code that you want to run. Alternatively, you can put `#!/usr/bin/env csto`
at the top of your script, run `chmod +x` on it, and then you can run the
script as if it was a standalone executable, for example `./yourscript.lua`.

## Documentation

Docs can be found here:
https://jtbx.github.io/callisto/doc

## Contributing

Pop me an email with your patch at jtbx@disroot.org or open a Github pull request.
