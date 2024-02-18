include config.mk

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man
LUADIR = external/lua

CC       = ${_CC}
CFLAGS   = ${_CFLAGS} -I${LUADIR} -pedantic -Wall -Wextra
CPPFLAGS = -D_DEFAULT_SOURCE
LDFLAGS  = -lm ${_LDFLAGS}

OBJS = callisto.o lcl.o lenviron.o lextra.o lfs.o ljson.o \
       lprocess.o util.o
LIBS = liblua.a

CJSON_SRC    = external/json
CJSON_OBJS   = fpconv.o lua_cjson.o strbuf.o
CJSON_CFLAGS = ${_CFLAGS} -I${LUADIR}

all: csto libcallisto.a

csto: ${LIBS} libcallisto.a csto.o
	${CC} -o $@ csto.o libcallisto.a ${LIBS} ${LDFLAGS}
libcallisto.a: liblua.a ${CJSON_OBJS} ${OBJS}
	ar cr $@ ${OBJS} ${CJSON_OBJS}

.SUFFIXES: .o

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c $<

csto.o: csto.c callisto.h
callisto.o: callisto.c callisto.h
lcl.o: lcl.c callisto.h util.h
lextra.o: lextra.c callisto.h util.h
lenviron.o: lenviron.c callisto.h
lfs.o: lfs.c callisto.h util.h
ljson.o: ljson.c callisto.h
lprocess.o: lprocess.c callisto.h util.h
	${CC} ${CFLAGS} -Wno-override-init ${CPPFLAGS} -c lprocess.c
util.o: util.c

# cjson
fpconv.o: ${CJSON_SRC}/fpconv.c
	${CC} ${CJSON_CFLAGS} -c $<
lua_cjson.o: ${CJSON_SRC}/lua_cjson.c
	${CC} ${CJSON_CFLAGS} -c $<
strbuf.o: ${CJSON_SRC}/strbuf.c
	${CC} ${CJSON_CFLAGS} -c $<

liblua.a: external/lua/*.c
	${MAKE} -Cexternal/lua
	mv -f external/lua/liblua.a .

clean:
	rm -f csto libcallisto.a csto.o ${OBJS} ${CJSON_OBJS} ${LIBS}
	rm -fr include doc/*.html doc/modules
	${MAKE} -s -Cexternal/lua clean

doc:
	ldoc -s . -q . >/dev/null

install:
	mkdir -p include/callisto
	mkdir -p "${DESTDIR}${PREFIX}"/{bin,include,lib}
	cp -f callisto.h include/callisto
	cp -f external/lua/{lua.h,lualib.h,lauxlib.h,luaconf.h} include/callisto
	cp -f csto "${DESTDIR}${PREFIX}"/bin
	cp -fR include/callisto "${DESTDIR}${PREFIX}"/include
	cp -f libcallisto.a "${DESTDIR}${PREFIX}"/lib

.PHONY: all clean doc install
