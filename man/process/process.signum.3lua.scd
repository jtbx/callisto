PROCESS.SIGNUM(3lua) "3lua" "Callisto manual pages"

# NAME
*process.signum* - translate a signal's name into its numeric constant

# SYNOPSIS
*process.signum*(_sig_: *string*)

# DESCRIPTION
*process.signum* returns the numeric form of the signal name _sig_.
The returned integer will likely differ across platforms.

On many platforms, *SIGKILL* has the numeric value *9*;
therefore, on those platforms _process.signum("SIGKILL")_
would return *9*.

If _sig_ is not the name of a valid signal,
*process.signum* throws a Lua error.

# EXAMPLES
Print the value of *SIGUSR1*:

	print("SIGUSR1 is " .. process.signum("SIGUSR1"))

# SEE ALSO
callisto(3lua), process(3lua)
