FS.BASENAME(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.basename* - get the last component of a path

# SYNOPSIS
*fs.basename*(_path_: *string*)

# DESCRIPTION
*fs.basename* returns the last component of _path_,
removing any trailing slash "/" characters.
If _path_ consists entirely of slash characters, the string "/" is
returned.
If _path_ is an empty string, the string "." is returned.

This is purely a string manipulation function
and depends on no outside state.

# EXAMPLES
Print the name of the currently running script file:

	print("name of script file is " .. fs.basename(arg[0]))

Output example:

	assert(fs.basename("/etc/fstab") == "fstab")

# SEE ALSO
callisto(3lua), fs(3lua)
