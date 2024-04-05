FS.DIRNAME(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.dirname* - get the parent directory of a path

# SYNOPSIS
*fs.dirname*(_path_: *string*)

# DESCRIPTION
*fs.dirname* returns the parent directory of _path_.

Any trailing "/" characters found in _path_
are not counted as part of the directory name.
If _path_ is an empty string or contains no "/" characters,
the string "." is returned.

This is purely a string manipulation function
and depends on no outside state.

# EXAMPLES
Print the directory containing the current script file:

	print(fs.dirname(arg[0]))

Output example:

	assert(fs.dirname("/usr/local/bin") == "/usr/local")

# SEE ALSO
callisto(3lua), fs(3lua)
