#!/usr/bin/env sh
# usage: scripts/install-lua.sh lua-x.x.x.tar.gz
# Install a Lua release into the Callisto source tree.

set -e

[ -z "$1" ] && (echo "usage: $0 lua-x.x.x.tar.gz"; exit 1)
! [ -d external/lua ] && (echo "not in Callisto source tree"; exit 1) || true

# clear out existing Lua sources
cd external/lua/
for file in $(ls -A | sed -Ee 's/^(patches|.gitignore)$//'); do
	rm -fr "$file"
done
cd ../../

# install new Lua sources
luadir="$(tar -tf "$1" | head -n1)"
rm -rf "$luadir"
tar -xf "$1"
ls "$luadir"
mv "$luadir"/README "$luadir"/src/
cp -R "$luadir"/src/* external/lua/
rm -r "$luadir"

# patch the sources
cd external/lua
for patch in $(ls patches/); do
	patch -p0 -i patches/"$patch"
done
cd ../../
