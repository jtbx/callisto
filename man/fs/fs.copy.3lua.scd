FS.COPY(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.copy* - copy a file

# SYNOPSIS
*fs.copy*(_source_: *string*, _target_: *string*)

# DESCRIPTION
*fs.copy* copies the contents of the _source_ file to the _target_ file,
overwriting any existing contents of the _target_ file.

It can be considered analogous to the *cp* shell command.

The call _fs.copy("src", "trg")_ is roughly equivalent to the following
Lua code:

```
local sf, se = assert(io.open("src", 'r'))
local tf, te = assert(io.open("trg", 'w'))

repeat
	local readbuf = readbuf or ""
	tf:write(readbuf)
	readbuf = sf:read(512)
until not readbuf
```

# EXAMPLES
Create a file _tel_ and write some data to it,
and copy its contents to the file _tel2_:

```
local contents = [[
maja    0501-1136285
peter   0136-7399214
]]
io.output("tel"):write(contents)
fs.copy("tel", "tel2")
assert(io.input("tel2"):read("a") == contents)
```

# SEE ALSO
callisto(3lua), fs(3lua), fs.move(3lua)
