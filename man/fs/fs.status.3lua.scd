FS.STATUS(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.status* - get the status of a file

# SYNOPSIS
*fs.status*(_path_: *string*)

# DESCRIPTION
*fs.status* retrieves information about the file pointed to by _path_
and returns a table with fields representing this information.
No prior permission is required on _path_ itself,
but all directories in the pathname leading to the file
must be searchable.

The returned table has the following fields:

- *path* (*string*): The path provided to *fs.status*.
- *mode* (*integer*): The file's mode; see *Modes* below.
- *uid* (*integer*): UID of the file's owner.
- *gid* (*integer*): GID of the file's group.
- *accessdate* (*integer*): Time the file's data was last accessed.
- *modifydate* (*integer*): Time the file's data was last modified.
- *chdate* (*integer*): Time the file's status last changed.

Times are in the form of integers representing the amount of seconds
passed since January 1, 1970.

## Modes
The _mode_ field of the returned structure is a bitmask created by
ORing different mode values found in chmod(2).
These mode values are made available to Lua through constants
in the *fs* table, with the _S\__ prefix removed.
For example, the _S\_IXOTH_ constant mentioned in chmod(2)
can be referenced in Lua with _fs.IXOTH_.

To find the type of a file from its mode, bitwise AND the mode with
_fs.IFMT_ and compare the result with one of the following constants
in the *fs* library:

- *IFBLK*: Block special.
- *IFCHR*: Character special.
- *IFIFO*: FIFO special.
- *IFREG*: Regular.
- *IFDIR*: Directory.
- *IFLNK*: Symbolic link.
- *IFSOCK*: Socket.

## Errors
If an error occurs,
*fs.status* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.status* can fail;
see lstat(2) for a list of possible error values.

# EXAMPLES
Print the UIDs of every file in a table:

```
files = {"tel1", "tel2"}
for file in ipairs(files) do
	local st = fs.status(file)
	print(st.uid, st.path)
end
```

Output whether _/sbin_ is a directory:

	print(fs.status("/sbin").mode & fs.IFMT == fs.IFDIR)

On some systems with a merged /usr, this will output false.
Changing _IFDIR_ to _IFLNK_ in this example should output true
in that case. 

# SEE ALSO
callisto(3lua), fs(3lua), fs.type(3lua), lstat(2)
