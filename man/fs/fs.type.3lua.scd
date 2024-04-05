FS.TYPE(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.type* - get a file's type

# SYNOPSIS
*fs.type*(_path_: *string*)

# DESCRIPTION
*fs.type* returns a string describing the type of the file
pointed to by _path_.

The following strings can be returned:

- *"block"*: _path_ is a block device
- *"character"*: _path_ is a character device
- *"directory"*: _path_ is a directory
- *"fifo"*: _path_ is a FIFO
- *"file"*: _path_ is a regular file
- *"socket"*: _path_ is a Unix domain socket
- *"symlink"*: _path_ is a symbolic link

If an error occurs,
*fs.type* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.type* can fail;
see lstat(2) for a list of possible error values.

# EXAMPLES
Print the type of /dev/urandom (on Linux and OpenBSD):

	print(fs.type("/dev/urandom")) --> character

# SEE ALSO
callisto(3lua), fs(3lua), fs.status(3lua), lstat(2)
