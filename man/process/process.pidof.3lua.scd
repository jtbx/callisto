PROCESS.PIDOF(3lua) "3lua" "Callisto manual pages"

# NAME
*process.pidof* - get the PID of another process

# SYNOPSIS
*process.pidof*(_cmd_: *string*)

# DESCRIPTION
*process.pidof* returns the PID of a process
which has the command name _cmd_.
The process is chosen arbitrarily; if there are multiple matches,
the first process found with the command name _cmd_ is returned.

If a process could not be found, *process.pidof* returns nil.

# EXAMPLES
Flawed implementation of *process.pid*:

```
function pid()
	return process.pidof("csto")
end
```

# SEE ALSO
callisto(3lua), process(3lua)
