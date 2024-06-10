## callisto - standalone scripting platform for Lua 5.4

[![builds.sr.ht status](https://builds.sr.ht/~jeremy/callisto.svg)](https://builds.sr.ht/~jeremy/callisto)

Callisto extends Lua 5.4's standard library by adding new modules and
facilities to the interpreter. It includes a file system library to
manage and manipulate files and directories, a process library to find
active processes and manipulate signals, and a JSON manipulation
library (lua-cjson) *among many more*.

It is a standalone interpreter designed for people using Lua as a
general scripting language, rather than using it embedded into another
program, which is what Lua was designed for. Perhaps you could think
of it as a replacement for Python.

First and foremost, Callisto tries to be:
 - a runtime environment for Lua that includes most features people
   would need out of the box, all in one executable with zero
   dependencies
 - a library that works well and integrates well with the Lua language
   and its standard library, and is easy to use for those who have no
   prior experience with C

## portability

To build Callisto, all you need is a C99 compiler. The configure
script will check for the presence of various compilers before
building, to decide which one to use.
The compilers checked are clang, followed by gcc, followed by cc. If
you have a compiler at a custom path that you would like to use over
the system C compiler, just pass `-c /path/to/compiler` to the
configure script before you build. The compiler must support gcc-like
command line arguments.

Callisto has zero runtime dependencies, unless you built it with
support for GNU libreadline [1]. Lua 5.4 is statically linked in. This
means that the same binary will work across different Linux
distributions.

Callisto relies on APIs standardized by POSIX.1-2017; therefore it
cannot be used on operating systems that do not comply with this
specification (like Microsoft Windows).

Theoretically Callisto should work on all operating systems
implementing POSIX. It has been tested on Linux and OpenBSD; but if
you find Callisto unable to build on your platform, please let me
know. Send an email to jeremy@baxters.nz or file a ticket by emailing
~jeremy/callisto@todo.sr.ht.

[1]: libreadline support can be enabled at build time, but is disabled by
default. To force building with libreadline support, pass the
`-wreadline` flag to the configure script.

## installation

Callisto is distributed as source-only, but it's very easy to build it
yourself. Just make sure you have a compiler such as gcc installed, as
well as git for downloading the source code and make for compiling.

First, obtain a copy of the source code using the following command:

    git clone https://git.sr.ht/~jeremy/callisto

After that, run

    ./configure
    make

to compile Callisto and all its dependencies.

To install it, run `make install` as the root user in the source code
directory. If you choose not to install it, you can still invoke it in
the current directory.

### Arch Linux

Users of Arch Linux can install the AUR package:

https://aur.archlinux.org/packages/callisto-git

### Nix

If you use Nix, you can use the flake:

    nix profile install sourcehut:~jeremy/callisto

## usage

The standalone Callisto interpreter is called `csto`. This is the main
way to run Lua programs using the Callisto libraries. Running it
without arguments will start a read-eval-print loop so you can execute
chunks of code interactively.

csto works just like the standalone Lua 5.4 interpreter. To execute
code in a file, run `csto file` where *file* is the name of the file
containing code you want to run.

Alternatively, put `#!/usr/bin/env csto` at the top of your script,
make it executable with `chmod +x`, and then you can run the script as
if it was a standalone executable, for example `./script.lua`.

## documentation

Library documentation can be found here:
https://jtbx.github.io/callisto/doc

## bugs

If you find a bug, please submit a report using the ticket tracker:
  https://todo.sr.ht/~jeremy/callisto

If you don't have a sourcehut account, you can file a ticket by sending
an email to ~jeremy/callisto@todo.sr.ht.

## contributing

Send a patch to ~jeremy/callisto-devel@lists.sr.ht using
[`git send-email`](https://git-send-email.io). Preferred code style is
detailed in the style.md document but is not required. Thanks :)
