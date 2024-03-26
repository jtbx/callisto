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
	assert(type(f) == "function")
	printfmt(f())
end

-- Table containing tests for each library.
-- The cl library is excluded from here as it is almost
-- impossible to test without user interaction.
local tests = {
	environ = {
		get = function ()
			local var = "HOME"

			assert(environ[var])
			return 'environ["' .. var .. '"] ~= nil'
		end,
		set = function ()
			local var = "VAR"
			local val = "hello"

			environ[var] = val
			assert(environ[var] == "hello")
			return 'environ["' .. var .. '"] = "' .. val .. '"'
		end,
		pairs = function ()
			local ev = {}
			local var = "VAR"

			environ[var] = "1"
			for env in pairs(environ) do
				ev[env] = environ[env]
			end
			assert(ev[var] == "1")
			return "pairs(environ)"
		end
	},

	extra = {
		printfmt = function ()
			printfmt("Testing %s (%d)", "printfmt", os.time())
			return 'printfmt("Testing %%s (%%d)", ...)'
		end
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

			-- leftovers from last time?
			if fs.isdirectory(dir) then
				fs.remove(dir)
			end

			assert(fs.mkdir(dir))
			assert(fs.exists(dir) and fs.isdirectory(dir)
				and fs.type(dir) == "directory")
			assert(fs.rmdir(dir))
			assert(fs.mkpath(subdir))

			assert(fs.exists(dir) and fs.isdirectory(dir))
			assert(fs.exists(subdir) and fs.isdirectory(subdir))

			assert(fs.remove(dir))

			return ([[
fs.mkdir("%s")
fs.rmdir("%s")
fs.mkpath("%s")
fs.remove("%s")]]):format(
				dir,
				dir,
				subdir,
				dir)
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
			local bpaths = {
				{"/etc/fstab", "fstab"},
				{"/sbin/init", "init"},
				{"init", "init"}
			}
			local dpaths = {
				{"/usr/local/bin", "/usr/local"},
				{"test.lua", "."}
			}
			local res = {}

			for _, entry in ipairs(bpaths) do
				assert(fs.basename(entry[1]) == entry[2])
				res[#res + 1] = '\nfs.basename("' .. entry[1] .. '")'
			end
			for _, entry in ipairs(bpaths) do
				assert(fs.basename(entry[1]) == entry[2])
				res[#res + 1] = '\nfs.dirname("' .. entry[1] .. '")'
			end

			res[1] = res[1]:sub(2) -- strip leading newline
			return table.concat(res)
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
			local d = "/etc"
			local wd = fs.workdir()

			assert(type(wd) == "string")
			fs.workdir(d)
			assert(fs.workdir() == d)
			fs.workdir(wd)
			assert(fs.workdir() == wd)
			return 'fs.workdir("' .. d .. '")'
		end
	},

	os = {
		hostname = function ()
			assert(type(os.hostname()) == "string")
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

			assert(math.type(process.signum(sig)) == "integer")

			-- https://todo.sr.ht/~jeremy/callisto/2
			local ok, err = pcall(function ()
				process.signum("SIGHELLO")
			end)
			assert(not ok)
			assert(err:find("no such signal$"))

			return 'process.signum("' .. sig .. '")'
		end,
		send = function ()
			local hdl = io.popen("sleep 2")
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
	test(environ.get)
	test(environ.set)
	test(environ.pairs)

	-- extra
	test(extra.printfmt)

	-- fs
	test(fs.copy)
	test(fs.directory)
	test(fs.move)
	test(fs.path)
	test(fs.remove)
	test(fs.workdir)

	-- os
	test(os.hostname)

	-- process
	test(process.pid)
	test(process.pidof)
	test(process.signum)
	test(process.send)
end

cl.mesg("all tests completed successfully")
