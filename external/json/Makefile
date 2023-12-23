##### Available defines for CJSON_CFLAGS #####
##
## USE_INTERNAL_ISINF:      Workaround for Solaris platforms missing isinf().
## DISABLE_INVALID_NUMBERS: Permanently disable invalid JSON numbers:
##                          NaN, Infinity, hex.
##
## Optional built-in number conversion uses the following defines:
## USE_INTERNAL_FPCONV:     Use builtin strtod/dtoa for numeric conversions.
## IEEE_BIG_ENDIAN:         Required on big endian architectures.
## MULTIPLE_THREADS:        Must be set when Lua CJSON may be used in a
##                          multi-threaded application. Requries _pthreads_.

##### Build defaults #####
CC     = cc
AR     = ar cr
CFLAGS = -std=c99 -O2 -Wall -Wno-unused-function -pedantic -fpic -DNDEBUG -I../luasrc
OBJS   = dtoa.o fpconv.o g_fmt.o lua_cjson.o strbuf.o

##### End customisable sections #####

.PHONY: all clean

all: cjson.a

cjson.a: ${OBJS}
	${AR} $@ ${OBJS}

dtoa.o:
	${CC} ${CFLAGS} -o $@ -c dtoa.c
fpconv.o:
	${CC} ${CFLAGS} -o $@ -c fpconv.c
g_fmt.o:
	${CC} ${CFLAGS} -o $@ -c g_fmt.c
lua_cjson.o:
	${CC} ${CFLAGS} -o $@ -c lua_cjson.c
strbuf.o:
	${CC} ${CFLAGS} -o $@ -c strbuf.c

clean:
	rm -f cjson.a ${OBJS}
