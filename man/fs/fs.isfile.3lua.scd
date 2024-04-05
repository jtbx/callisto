FS.ISFILE(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.isfile* - check whether a path points to a regular file

# SYNOPSIS
*fs.isfile*(_path_: *string*)

# DESCRIPTION
*fs.isfile* returns whether _path_ points to a regular file
(i.e. not a directorym symbolic link, or character/block device).

If an error occurs,
*fs.isfile* returns nil, an error message and
a platform-dependent error code.
Usually this only happens when passing a nonexistent path,
or a path to a file which you do not have permission to read.
See lstat(2) for a complete list.

# EXAMPLES
Make sure _tel_ is a regular file:

	if not fs.isfile("tel") then
		print("tel is not a regular file")
	end

# SEE ALSO
callisto(3lua), fs(3lua), fs.isdirectory(3lua), fs.issymlink(3lua)
