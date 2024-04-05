FS.MKDIR(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.mkdir* - create a directory

# SYNOPSIS
*fs.mkdir*(_path_: *string*)

# DESCRIPTION
*fs.mkdir* creates a new directory at _path_.

If you're trying to create multiple directories at once,
you should use fs.mkpath(3lua) instead.

If an error occurs,
*fs.mkdir* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.mkdir* can fail;
see mkdir(2) for a list of possible error values.

# EXAMPLES
Create the user's home directory:

	fs.mkdir(environ["HOME"])

# SEE ALSO
callisto(3lua), fs(3lua), fs.mkpath(3lua), mkdir(2)
