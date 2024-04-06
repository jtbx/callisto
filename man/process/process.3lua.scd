PROCESS(3lua) "3lua" "Callisto manual pages"

# NAME
*process* - process and signal library

# SYNOPSIS
.Syn

# DESCRIPTION
The *process* library provides routines
to find processes and manipulate them by sending signals.

Signals are passed as strings identifying the signal,
which represent the signal's name as defined in your system's
*<signal.h>*.
Signals are converted from their string name into
the system-dependent numerical constant.
To perform this manually, see process.signum(3lua).

The term _PID_ refers to a process ID;
the unique integer assigned to a process upon execution.

# SEE ALSO
callisto(3lua)
