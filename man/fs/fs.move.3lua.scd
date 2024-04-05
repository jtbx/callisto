FS.MOVE(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.move* - move a file

# SYNOPSIS
*fs.move*(_src_: *string*, _dst_: *string*)

# DESCRIPTION
*fs.move* moves the file _src_ to the path _dst_,
overwriting _dst_ if it exists.
Both _src_ and _dst_ must be of the same type
(that is, both directories or both non-directories)
and must reside on the same file system.

If you need to move files across different file systems,
consider using fs.copy(3lua) instead.

If an error occurs,
*fs.move* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.move* can fail;
see rename(2) for a list of possible error values.

# EXAMPLES
Rename the file _tel_ to _tel1_:

	fs.move("tel", "tel1")

# SEE ALSO
callisto(3lua), fs(3lua), fs.copy(3lua), rename(2)
