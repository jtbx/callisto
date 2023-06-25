project = "Callisto"
title = "Callisto library documentation"
description = "Callisto - A featureful runtime for Lua 5.4"
full_description = [[
Callisto is a runtime for Lua 5.4 that includes
many useful libraries relating to file manipulation,
processes and signals, networking & sockets,
command-line option parsing, environment
variables and more. It is designed to make
Lua into a general purpose standalone scripting
language, but that doesn't mean you can't embed
it into your own program.
]]
dir = "doc"
format = "markdown"
style = "."

file = {
	".",
	exclude = {
		"external",
		"lua-5.4"
	}
}

new_type("setting", "Settings")

-- vi: ft=lua
