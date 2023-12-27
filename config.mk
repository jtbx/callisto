PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

CC       = cc
CFLAGS   = -std=c99 -pedantic -fpic -O2 -Wall -Wextra -Wno-override-init -Iexternal/lua
CPPFLAGS = -D_DEFAULT_SOURCE
LDFLAGS  = -lm

# Enable readline
#CPPFLAGS += -DLUA_USE_READLINE
#LDFLAGS  += -lreadline
