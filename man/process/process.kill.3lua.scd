PROCESS.KILL(3lua) "3lua" "Callisto manual pages"

# NAME
*process.kill* - kill a process

# SYNOPSIS
*process.kill*(_pid_: *integer*)

# DESCRIPTION
*process.kill* sends *SIGKILL* to the process identified by _pid_.

Equivalent to _process.send(pid, "SIGKILL")_, and fails for all the
reasons *process.send* fails for.

# SEE ALSO
callisto(3lua), process(3lua), process.send(3lua)
