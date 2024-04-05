FS.EXISTS(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.exists* - check existence of a path in the file system

# SYNOPSIS
*fs.exists*(_path_: *string*)

# DESCRIPTION
*fs.exists* returns whether the pathname _path_ exists
in the file system.

# EXAMPLES
Check whether _tel_ exists:

	if not fs.exists("tel") then
		print("tel does not exist")
	end

# SEE ALSO
callisto(3lua), fs(3lua), fs.isfile(3lua)
