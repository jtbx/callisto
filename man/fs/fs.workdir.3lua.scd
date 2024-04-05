FS.WORKDIR(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.workdir* - get or set the current working directory

# SYNOPSIS
*fs.workdir*([_dir_: *string*])

# DESCRIPTION
If _dir_ is not nil, *fs.workdir* sets the current working directory
to the path _dir_.
This means the starting point of all pathnames not beginning with a
slash ('/') will be set to _dir_ until the working directory is
changed again by a call to *fs.workdir* or chdir(2).

If _dir_ is nil, *fs.workdir* returns the current working directory.

If an error occurs,
*fs.workdir* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.workdir* can fail;
see chdir(2) for possible error values
when setting the current working directory,
and getcwd(2) for possible error values
when getting the current working directory.

# EXAMPLES
Get the current working directory:

	local wd = fs.workdir()

Set the current working directory to the user's home directory:

	fs.workdir(environ["HOME"])

# SEE ALSO
callisto(3lua), fs(3lua), chdir(2), getcwd(2)
