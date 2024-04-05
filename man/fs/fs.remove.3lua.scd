FS.REMOVE(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.remove* - remove a file or directory

# SYNOPSIS
*fs.remove*(_path_: *string*)

# DESCRIPTION
*fs.remove* removes the file or directory at _path_.

If _path_ specifies a file, *fs.remove* removes the file.
If _path_ specifies a directory, *fs.remove* recursively removes any
other files or directories inside the directory given,
and then removes the directory itself once it is empty.

If an error occurs,
*fs.remove* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.remove* can fail;
see rmdir(2) and unlink(2) for possible error values.

## Warning
This function is capable of destroying large amounts of data on a disk
very quickly.
In case of data damage using *fs.remove* or other means, consider
using data recovery software such as testdisk(8).

# EXAMPLES
Remove the _tel2_ file:

	fs.remove("tel2")

# SEE ALSO
callisto(3lua), fs(3lua), fs.rmdir(3lua), unlink(2), rmdir(2)
