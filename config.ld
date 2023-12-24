project = "Callisto"
title = "Callisto library documentation"
description = "standalone scripting platform for Lua 5.4"
full_description = [[
Callisto is a standalone scripting environment for Lua 5.4
which includes many useful libraries relating to file system
manipulation, processes and signals, command-line argument
parsing, environment variables and more. It is designed to
make Lua into a general purpose standalone scripting language
that anyone can use.
]]
dir = "doc"
format = "markdown"
style = "."

file = {
	".",
	exclude = {"external"}
}

new_type("setting", "Settings")

-- vi: ft=lua
