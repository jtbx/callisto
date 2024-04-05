FS.RMDIR(3lua) "3lua" "Callisto manual pages"

# NAME
*fs.rmdir* - remove an empty directory

# SYNOPSIS
*fs.rmdir*(_dir_: *string*)

# DESCRIPTION
*fs.rmdir* removes the empty directory _dir_.

If you need to recursively remove a directory containing other files
or directories, you should use fs.remove(3lua) instead.

If an error occurs,
*fs.rmdir* returns nil, an error message and
a platform-dependent error code.
There are many reasons *fs.rmdir* can fail;
see rmdir(2) for a list of possible error values.

# EXAMPLES
Remove the empty directory _mydir_:

```
assert(fs.mkdir("mydir"))
fs.rmdir("mydir")
```

# SEE ALSO
callisto(3lua), fs(3lua), fs.remove(3lua), rmdir(2)
