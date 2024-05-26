-- mkpages.lua: generate Callisto man pages

-- This script will fill in the SYNOPSIS section of top-level man pages
-- in order for them to be complete. For instance, it will find the
-- fs.copy(3lua) and fs.mkdir(3lua) man pages in the fs/ subdirectory
-- and build a man page for fs(3lua) containing references to fs.copy(3lua)
-- and fs.mkdir(3lua).
--
-- As well as the above, this script will run scdoc over all man pages
-- found in these directories and place them in `outdir', defined below.

assert(_CALLISTOVERSION)

local defect = "*Documentation defect*"
local outdir = "man3lua"

-- returns whether the list t has a value equal to v
local function has(v, ...)
	for _, n in ipairs(table.pack(...)) do
		if n == v then
			return true
		end
	end
	return false
end

-- returns the contents of filename
local function readFile(filename)
	local f, err = io.open(filename, 'r')

	assert(f, err)
	return f:read("*a")
	-- getmetatable(f).__gc()
end

local function sh(fstring, ...)
	local c = fstring:format(...)
	printfmt("%s", c)
	return assert(os.execute(c))
end

-- returns the contents of filename
local function writeToFile(filename, contents)
	local f, err = io.open(filename, 'w')

	assert(f, err)
	f:write(contents)
	f:close()
end

local StringBuffer = {}
function StringBuffer:assemble()
	assert(type(self._buf) == "table")
	local buf = table.concat(self._buf)
	self._buf = nil
	return buf
end
function StringBuffer:buffer()
	return self._buf
end
function StringBuffer:push(o)
	assert(type(self._buf) == "table")
	assert(type(o) == "string")
	self._buf[#self._buf + 1] = o
end
function StringBuffer:new()
	local o = {}
	o._buf = {}
	setmetatable(o, self)
	self.__index = self
	return o
end

-- returns a table containing the fields "name" and "synopsis",
-- which are strings containing the first line of the "NAME" and
-- "SYNOPSIS" sections of the scdoc man page at the given path
local function parseScd(filename)
	local f, err = io.open(filename, 'r')
	local inName = false
	local inSynopsis = false
	local name = defect
	local synopsis = defect

	assert(f, err)

	for line in f:lines() do
		if inName then
			name = line
			inName = false
		elseif inSynopsis then
			synopsis = line
			inSynopsis = false
		end
		if line == "# NAME" then
			inName = true
		elseif line == "# SYNOPSIS" then
			inSynopsis = true
		end
	end

	return {
		name = name,
		synopsis = synopsis
	}
end

-- entry point

local indexpaths = {}
local libs = {}
local libnames = StringBuffer:new()

-- search out all manual page directories
for dir in fs.entries() do
	local ty = fs.type(dir.path)

	if has(dir.path, ".", "..", "man1", outdir)
	or fs.type(dir.path) ~= "directory" then
		goto nextdir
	end

	assert(fs.isfile(dir.path .. '/' .. dir.path .. ".3lua.scd"),
		"index page for " .. dir.path .. " not found")

	libs[dir.path] = StringBuffer:new()
	indexpaths[dir.path] = dir.path .. '/' .. dir.path .. ".3lua.scd"

	-- search for all manual pages
	for entry in fs.entries(dir.path) do
		local path = dir.path .. '/' .. entry.path

		if fs.type(path) ~= "file"
		or entry.path == dir.path .. ".3lua.scd"
		or not entry.path:find("^.+%.3lua%.scd$") then
			goto nextentry
		end

		local doc = parseScd(path)
		libs[dir.path]:push(
			string.format("%s\n\t%s\n\n", doc.name, doc.synopsis))

		::nextentry::
	end

	table.sort(libs[dir.path]:buffer())
	libs[dir.path] = libs[dir.path]:assemble()
	::nextdir::
end

if fs.exists(outdir) then
	fs.remove(outdir)
end

local libindex = StringBuffer:new()

for lib in pairs(libs) do
	local orig = parseScd(lib .. '/' .. lib .. '.3lua.scd').name
	local name = ""
	local desc = ""
	local curr = ""

	for ch in orig:gmatch(".") do
		if ch == '*' then
			goto nextchar
		elseif ch == '-' then
			name = curr:sub(1, -2)
			curr = ""
		else
			curr = curr .. ch
		end

		::nextchar::
	end

	assert(#name > 0)
	assert(#curr > 0)
	desc = curr:sub(2)

	libindex:push((".It Sy %s\n%s\n"):format(name, desc))
end

libindex = libindex:assemble()
fs.mkdir(outdir)

-- generate callisto.3lua
do
	local callisto3lua = readFile("callisto.3lua")
		:gsub("{%.Lbs}", libindex:sub(1, #libindex - 1))
	writeToFile(outdir .. "/callisto.3lua", callisto3lua)
end

-- generate all other 3lua pages
for lib, index in pairs(libs) do
	assert(type(lib) == "string")
	assert(type(index) == "string")
	assert(fs.isdirectory(lib))

	local p = io.popen(("scdoc > %s/%s.3lua"):format(outdir, lib), 'w')
	p:write(readFile(indexpaths[lib])
		:gsub("%.Syn", index:sub(1, #index - 2), 1))
	assert(p:close())

	for manpage in fs.entries(fs.dirname(indexpaths[lib])) do
		local name = manpage.path
		local fullpath = fs.dirname(indexpaths[lib]) .. '/' .. name

		if has(name, ".", "..")
		or fs.type(fullpath) ~= "file"
		or not name:find("^.+%.3lua%.scd$") then
			goto nextpage
		end

		sh("scdoc < %s > %s",
			fullpath, outdir .. '/' .. name:sub(1, #name - 4))

		::nextpage::
	end
end
