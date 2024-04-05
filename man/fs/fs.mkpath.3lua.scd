FS.MKPATH(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.mkpath* - create a tree of directories

# SYNOPSIS
*fs.mkpath*(_path_: *string*)

# DESCRIPTION
*fs.mkpath* creates multiple directories specified by one path.
For example, _fs.mkpath("/usr/local/share/man/man3lua")_ would first
check that _/usr_ is a directory, and if not attempt to create it, and
then repeat that same process for each following component of the path,
similar to the behaviour of mkdir(1)'s *-p* option.

For creating single directories without this advanced functionality,
it might better suit you to use fs.mkdir(3lua) instead.

If an error occurs,
*fs.mkpath* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.mkpath* can fail;
see mkdir(2) and stat(2) for possible error values.

# EXAMPLES
Create the directory _/usr/local/include/callisto_, creating
intermediate directories as required:

	fs.mkpath("/usr/local/include/callisto")

# SEE ALSO
callisto(3lua), fs(3lua), fs.mkdir(3lua), mkdir(2)
