--[[
    Tests for Callisto
    Run with:
       ./csto test.lua

    A test should use the smallest amount of functions possible,
    and should not rely on facilities such as os.execute to complete
    its task. Using functions in the same library is acceptable, but
    Callisto facilities in different libraries should not be used.

    Licensed to the public domain
]]--

local oldenv = _ENV

local function test(f)
	local _ENV = oldenv
	local function mesg(...)
		io.stdout:write(..., '\n')
	end

	assert(type(f) == "function")
	mesg(f())
end

-- Table containing tests for each library.
-- The cl library is excluded from here as it is almost
-- impossible to test without user interaction.
local tests = {
	environ = {
		getvar = function ()
			local var = "HOME"

			assert(environ[var])
			return 'environ["' .. var .. '"] ~= nil'
		end,
		setvar = function ()
			local var = "VAR"
			local val = "hello"

			environ[var] = val
			assert(environ[var] == "hello")
			return 'environ["' .. var .. '"] = "' .. val .. '"'
		end
	},

	extra = {
	},

	fs = {
		copy = function ()
			local src, dst = "testfile", "testfile.cp"
			local contents = "hello, world!"
			local f, err = io.open(src, 'w')

			assert(f, err)

			f:write(contents)
			f:close()

			assert(fs.copy(src, dst))
			assert(io.input(dst):read('a') == contents)

			assert(fs.remove(src))
			assert(fs.remove(dst))

			return 'fs.copy("' .. src .. '", "' .. dst .. '")'
		end,
		directory = function ()
			local dir, subdir = "testdir", "testdir/sub"

			assert(fs.mkdir(dir))
			assert(fs.exists(dir) and fs.isdirectory(dir))
			assert(fs.rmdir(dir))
			assert(fs.mkdir(subdir, true))

			assert(fs.exists(dir))
			assert(fs.isdirectory(dir))
			assert(fs.exists(subdir))
			assert(fs.isdirectory(subdir))

			assert(fs.rmdir(subdir))
			assert(fs.rmdir(dir))

			return ([[
fs.mkdir("%s")
fs.rmdir("%s")
fs.mkdir("%s", true)
fs.rmdir("%s"")
fs.rmdir("%s")]]):format(
				dir,
				dir,
				subdir,
				subdir,
				dir
			)
		end,
		move = function ()
			local src, dst = "testfile", "testfile.new"
			local contents = "hello, world!"
			local f, err = io.open(src, 'w')

			assert(f, err)

			f:write(contents)
			f:close()

			assert(fs.move(src, dst))
			assert(io.input(dst):read('a') == contents)

			assert(fs.remove(dst))

			return 'fs.move("' .. src .. '", "' .. dst .. '")'
		end,
		path = function ()
			local bpath, base = "/etc/fstab", "fstab"
			local dpath, dir = "/usr/local/bin", "/usr/local"

			assert(fs.basename(bpath) == base)
			assert(fs.dirname(dpath) == dir)

			return 'fs.basename("' .. bpath .. [[")
fs.dirname("]] .. dpath .. '")'
		end,
		remove = function ()
			local file = "testfile"
			local f, err = io.open(file, 'w')

			assert(f, err)

			f:write("")
			f:close()

			assert(fs.remove(file))

			return 'fs.remove("' .. file .. '")'
		end,
		workdir = function ()
			local d = "/usr"
			local wd = fs.workdir()

			fs.workdir(d)
			assert(fs.workdir() == d)
			fs.workdir(wd)
			assert(fs.workdir() == wd)
			return 'fs.workdir("' .. d .. '")'
		end
	},

	-- basic tests for lua-cjson; this is not
	-- my library so I won't test it extensively
	json = {
		decode = function()
			local o = [[
"hello": "world",
"n": 4,
"fpn": 2.4444449,
"a": [1, 2, 4, 8, 16]
]]
			local j = [[
{
	]] .. o .. [[,
	"o": {
		]] .. o .. [[
	}
}
]]

			local t = json.decode(j)

			for _, t in ipairs {t, t.o} do
				assert(t.hello == "world")
				assert(t.n == 4)
				assert(t.fpn == 2.4444449)
				assert(t.a[1] == 1)
				assert(t.a[2] == 2)
				assert(t.a[3] == 4)
				assert(t.a[4] == 8)
				assert(t.a[5] == 16)
			end

			return "json.decode('" .. j:gsub("%s", "") .. "')"
		end,
		encode = function()
			local o = {
				hello = "world",
				n = 4,
				fpn = 2.4444449,
				a = {1, 2, 4, 8, 16}
			}

			local j = json.encode(o)
			local t = json.decode(j)

			assert(t.hello == "world")
			assert(t.n == 4)
			assert(t.fpn == 2.4444449)
			assert(t.a[1] == 1)
			assert(t.a[2] == 2)
			assert(t.a[3] == 4)
			assert(t.a[4] == 8)
			assert(t.a[5] == 16)

			return "json.decode(json.encode({...}))"
		end
	},

	os = {
		hostname = function ()
			assert(os.hostname())
			return "os.hostname()"
		end
	},

	process = {
		pid = function ()
			assert(math.type(process.pid()) == "integer")
			return "process.pid()"
		end,
		pidof = function ()
			local proc = "csto"

			assert(math.type(process.pidof(proc)) == "integer")
			return 'process.pidof("' .. proc .. '")'
		end,
		signum = function ()
			local sig = "SIGKILL"

			-- probably signal no.9
			assert(math.type(process.signum(sig)) == "integer")
			return 'process.signum("' .. sig .. '")'
		end,
		send = function ()
			local hdl = io.popen("sleep 8")
			local pid = process.pidof("sleep")

			assert(process.send(pid, "SIGKILL"))
			hdl:close()

			return "process.send(" .. tostring(pid) .. ', "SIGKILL")'
		end
	}
}

-- Run all tests.
do
	local env = tests
	env.test = test
	local _ENV = env

	-- environ
	test(environ.getvar)
	test(environ.setvar)

	-- fs
	test(fs.copy)
	test(fs.directory)
	test(fs.move)
	test(fs.path)
	test(fs.remove)
	test(fs.workdir)

	-- json
	test(json.decode)
	test(json.encode)

	-- os
	test(os.hostname)

	-- process
	test(process.pid)
	test(process.pidof)
	test(process.signum)
	test(process.send)
end

cl.mesg("all tests completed successfully")
