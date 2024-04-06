PROCESS.SEND(3lua) "3lua" "Callisto manual pages"

# NAME
*process.send* - send a signal

# SYNOPSIS
*process.send*(_pid_: *integer*, _sig_: *string*)

# DESCRIPTION
*process.send* sends the signal named _sig_
to the process identitied by _pid_.

If an error occurs,
*process.send* returns nil, an error message and
a platform-dependent error code.
There are a few reasons *process.send* can fail;
see kill(2) for a list of possible error values.

# EXAMPLES
Send *SIGKILL* to a process with command name *bash*:

	process.send(process.pidof("bash"), "SIGKILL")

# SEE ALSO
callisto(3lua), process(3lua), kill(2)
