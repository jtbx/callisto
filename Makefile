include config.mk

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/man

LUADIR = external/lua

CLANG_FORMAT = clang-format

CC       = ${_CC}
CFLAGS   = ${_CFLAGS} -Iexternal -pedantic -Wall -Wextra
CPPFLAGS = -D_DEFAULT_SOURCE ${_CPPFLAGS}
LDFLAGS  = ${_LDFLAGS}

OBJS = callisto.o lcl.o lenviron.o lextra.o lfs.o ljson.o \
       lprocess.o util.o
HEADERS = callisto.h \
	${LUADIR}/lua.h \
	${LUADIR}/luaconf.h \
	${LUADIR}/lualib.h \
	${LUADIR}/lauxlib.h

CJSON_SRC    = external/json
CJSON_OBJS   = fpconv.o lua_cjson.o strbuf.o
CJSON_CFLAGS = ${_CFLAGS} -I${LUADIR}

all: csto libcallisto.a

csto: libcallisto.a csto.o
	${CC} -o $@ csto.o libcallisto.a ${LDFLAGS}
libcallisto.a: lua ${CJSON_OBJS} ${OBJS}
	ar cr $@ ${OBJS} ${CJSON_OBJS} ${_LUAOBJS}

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

lua:
	${MAKE} -C${LUADIR}

clean:
	rm -f csto libcallisto.a csto.o ${OBJS} ${CJSON_OBJS}
	rm -fr include doc/*.html doc/modules
	${MAKE} -s -C${LUADIR} clean

doc:
	ldoc -s . -q . >/dev/null

format:
	find . -maxdepth 1 -iname '*.c' -o -iname '*.h' \
		| xargs ${CLANG_FORMAT} -i

gitconfig:
	git config format.subjectPrefix "PATCH callisto"
	git config sendemail.to '~jeremy/public-inbox@lists.sr.ht'

install:
	mkdir -p "${DESTDIR}${PREFIX}"/bin
	mkdir -p "${DESTDIR}${PREFIX}"/include/callisto
	mkdir -p "${DESTDIR}${PREFIX}"/lib
	mkdir -p "${DESTDIR}${PREFIX}"/share/man/man1
	cp -f ${HEADERS} "${DESTDIR}${PREFIX}"/include/callisto/
	cp -f csto "${DESTDIR}${PREFIX}"/bin/
	cp -f libcallisto.a "${DESTDIR}${PREFIX}"/lib/
	cp -f man/man1/*.1 "${DESTDIR}${PREFIX}"/share/man/man1/

.PHONY: all clean doc format gitconfig install lua
