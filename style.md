## Callisto code style

C, Lua and makefile code should use tabs for indentation; this allows
one to set their own tab width preference in their editor. However,
when code has to be lined up over multiple lines, it should be lined
up according to 4-width indentation.

C code in Callisto should follow the OpenBSD style(9) document:
 - http://man.openbsd.org/style.9

Code in the Nix flake should follow the nixpkgs style guide:
 - https://github.com/NixOS/nixpkgs/blob/master/CONTRIBUTING.md#code-conventions

## Commits

Prepend your commit message with the name of the Lua library that your
commit primarily changes, for example if I were to add a new function
named `copy` to the `fs` library, I might use the commit message
"fs: add new function 'copy'".
If the commit does not change code part of a Lua library but still
changes code, pick a prefix relevant to the code you changed,
e.g. "tests: add test for fs.copy", "makefile: fix build on NetBSD",
"flake: remove darwin from platforms".
If the commit does not change code at all scrap the prefix.

Additionally follow these conventions taken from nixpkgs:

- Create a commit for each logical unit.

- Check for unnecessary whitespace with `git diff --check` before committing.

- If you have commits `pkg-name: oh, forgot to insert whitespace`: squash commits in this case. Use `git rebase -i`.

- For consistency, there should not be a period at the end of the commit message's summary line (the first line of the commit message).
