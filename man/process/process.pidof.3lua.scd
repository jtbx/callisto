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

# BUGS
*process.pidof* depends on the nonportable program pgrep(1).
This program can be found in the procps-ng package on Linux
included in most distributions, or as part of the base system
on OpenBSD, NetBSD and FreeBSD.

Work is being done to free *process.pidof* of this burden;
follow along with the tracking ticket for any updates
<*https://todo.sr.ht/~jeremy/callisto/7*>.

# EXAMPLES
Flawed implementation of *process.pid*:

```
function pid()
	return process.pidof("csto")
end
```

# SEE ALSO
callisto(3lua), process(3lua)
