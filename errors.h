/* Error messages */

#ifndef _ERRORS_H_

#define _ERRORS_H_

#define E_PATHNAMETOOLONG "pathname too long"
#define E_WORKDIRNOTVALID "working directory is no longer valid"
#define E_COMPNOTEXIST    "component of path does not exist"
#define E_DIRNOTEMPTY     "directory not empty"
#define E_MOUNTPOINT      "directory is busy"
#define E_NOSUCHDEST      "no such file or directory"
#define E_FTRAVERSE       "failed to traverse directory"
#define E_NOSUCHDIR       "no such directory"
#define E_STICKYDIR       "permission denied (sticky directory)"
#define E_INTFAULT        "internal error (EFAULT)"
#define E_MVPARENT        "cannot move a parent directory of pathname"
#define E_FEXISTS         "file exists"
#define E_MAXLINK         "maximum link count reached"
#define E_NOSPACE         "insufficient space left on file system"
#define E_NOTADIR         "pathname or a component of pathname is not a directory"
#define E_SYMLINK         "could not translate pathname; too many symbolic links"
#define E_DIFFFS          "pathnames are on different file systems"
#define E_DIRDOT          "last component of path is '.'"
#define E_ISADIR          "cannot move a file to the name of a directory"
#define E_IOERR           "I/O error"
#define E_NOMEM           "insufficent memory"
#define E_QUOTA           "file system quota reached"
#define E_PERM            "permission denied"
#define E_ROFS            "read-only file system"

#endif
