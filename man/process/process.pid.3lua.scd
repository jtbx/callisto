PROCESS.PID(3lua) "3lua" "Callisto manual pages"

# NAME
*process.pid* - get the current process's PID

# SYNOPSIS
*process.pid*()

# DESCRIPTION
*process.pid* returns the PID of the current interpreter process.

# EXAMPLES
Create a function that returns a unique name for a temporary file:

```
function tmpname(prefix)
	return "/tmp/" .. prefix .. '.' .. process.pid()
end
```

# SEE ALSO
callisto(3lua), process(3lua), getpid(2)
