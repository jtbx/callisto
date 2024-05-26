PRINTFMT(3lua) "3lua" "Callisto manual pages"

# NAME
*printfmt* - write formatted data to standard output

# SYNOPSIS
*printfmt*(_fstring_: *string*, _..._)

# DESCRIPTION
*printfmt* writes formatted data
specified by the format string _fstring_ to standard output,
behaving similarly to the built in Lua function *string.format*.

It is somewhat equivalent to:

	io.stdout:write(string.format(_fstring_, _..._), '\n')

# EXAMPLES
Output the values of a list in CSV format:

```
list = {2, 4, 8, 16}
printfmt("%d,%d,%d,%d", list[1], list[2], list[3], list[4])
```

Without using *printfmt* or other string formatting functions,
this example would require extensive concatenation.

# SEE ALSO
callisto(3lua), extra(3lua)
