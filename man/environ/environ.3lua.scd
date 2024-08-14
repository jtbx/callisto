ENVIRON(3lua) "3lua" "Callisto manual pages"

# NAME
*environ* - the process environment

# DESCRIPTION
The *environ* table contains a metatable allowing
indexes to get or set variables in the environment.
Variables and variable values are provided as strings.

The metatable's _\_\_index_ field returns
the value of the given variable.
This means any index into the *environ* table will yield the value of
the corresponding variable in the environment.

The metatable's _\_\_newindex_ field sets the given variable's value.
This means you can set any key in the *environ* table to set
the value of an environment variable.

The metatable's _\_\_pairs_ field returns an iterator function
allowing iteration over all variables in the environment using
the *pairs* library function.

# EXAMPLES
Get the value of the _HOME_ environment variable and use it to
set the _HOMEDIR_ environment variable:

	local home = environ["HOME"]
	environ["HOMEDIR"] = fs.dirname(home)

Print the values of every variable in the environment:

	for var in pairs(environ) do
		print(var .. " = " environ[var])
	end

# SEE ALSO
callisto(3lua), process(3lua)
