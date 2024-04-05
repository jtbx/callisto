FS.ENTRIES(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.entries* - iterate over a directory

# SYNOPSIS
*fs.entries*([_directory_: *string*])

# DESCRIPTION
*fs.entries* returns an iterator function which can be used to iterate
over the contents of a directory, so that the idiom

	for entry in fs.entries(_directory_)
		_code..._
	end

will run the code inside the loop for every directory entry.

Without any arguments *fs.entries* will iterate over the current
working directory but an argument can be provided to use a different
directory instead.

On each iteration, the iterator will return a table retrieved from
fs.status(3lua), containing information about the next file in the
directory such as its path, mode, owner, and modify date.

# EXAMPLES
List every file and directory in _/etc_ and its type as retrieved from
fs.type(3lua):

	for entry in fs.entries("/etc") do
		print(fs.type("/etc/" .. entry.path), entry.path)
	end

# SEE ALSO
callisto(3lua), fs(3lua), fs.status(3lua)
