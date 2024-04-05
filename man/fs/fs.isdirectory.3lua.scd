FS.ISDIRECTORY(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.isdirectory* - check whether a path points to a directory

# SYNOPSIS
*fs.isdirectory*(_path_: *string*)

# DESCRIPTION
*fs.isdirectory* returns whether _path_ points to a directory.

If an error occurs,
*fs.isdirectory* returns nil, an error message and
a platform-dependent error code.
Usually this only happens when passing a nonexistent path,
or a path to a file which you do not have permission to read.
See lstat(2) for a complete list.

# EXAMPLES
Check whether /bin is a directory:

	if not fs.isdirectory("/bin") then
		print("something's wrong...")
	end

# SEE ALSO
callisto(3lua), fs(3lua), fs.isfile(3lua)
