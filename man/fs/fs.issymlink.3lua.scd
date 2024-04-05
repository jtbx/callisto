FS.ISSYMLINK(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.issymlink* - check whether a path points to a symbolic link

# SYNOPSIS
*fs.issymlink*(_path_: *string*)

# DESCRIPTION
*fs.issymlink* returns whether _path_ points to a symbolic link
(see symlink(7)).

If an error occurs,
*fs.issymlink* returns nil, an error message and
a platform-dependent error code.
Usually this only happens when passing a nonexistent path,
or a path to a file which you do not have permission to read.
See lstat(2) for a complete list.

# EXAMPLES
Check whether /bin is a symbolic link:

	if fs.issymlink("/bin") then
		print("most likely merged /usr")
	end

# SEE ALSO
callisto(3lua), fs(3lua), symlink(7)
