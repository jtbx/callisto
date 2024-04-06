PROCESS.TERMINATE(3lua) "3lua" "Callisto manual pages"

# NAME
*process.terminate* - terminate a process

# SYNOPSIS
*process.terminate*(_pid_: *integer*)

# DESCRIPTION
*process.terminate* sends *SIGTERM* to the process identified by _pid_.

Equivalent to _process.send(pid, "SIGTERM")_, and fails for all the
reasons *process.send* fails for.

# SEE ALSO
callisto(3lua), process(3lua), process.send(3lua)
