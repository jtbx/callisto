PREFIX = /usr/local

include config.mk

OBJS = csto.o     \
	   callisto.o \
	   lcl.o      \
	   lenviron.o \
	   lextra.o   \
	   lfs.o      \
	   ljson.o    \
	   lpath.o    \
	   lprocess.o \
	   lsocket.o  \
	   util.o
LIBS = liblua.a cjson.a socket.a

all: csto libcallisto.so

csto: ${LIBS} ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS} ${LDFLAGS}
libcallisto.so: ${LIBS} ${OBJS}
	${CC} -shared ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.SUFFIXES: .o

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c $<

csto.o: csto.c callisto.h
callisto.o: callisto.c callisto.h
lcl.o: lcl.c callisto.h util.h
lextra.o: lextra.c callisto.h util.h
lenviron.o: lenviron.c callisto.h
lfs.o: lfs.c callisto.h errors.h util.h
ljson.o: ljson.c callisto.h
lpath.o: lpath.c callisto.h errors.h util.h
lprocess.o: lprocess.c callisto.h util.h
lsocket.o: lsocket.c callisto.h
util.o: util.c

cjson.a: external/json/*.c
	${MAKE} -Cexternal/json
	mv -f external/json/cjson.a cjson.a
socket.a: external/socket/src/*.c
	${MAKE} -Cexternal/socket
	mv -f external/socket/src/socket.a socket.a

liblua.a: lua-5.4/*.c
	${MAKE} -Clua-5.4
	mv -f lua-5.4/liblua.a .

clean:
	rm -f csto libcallisto.so ${OBJS} ${LIBS}
	rm -fr include
	rm -fr doc/*.html doc/modules
clean-all: clean
	${MAKE} -s -Clua-5.4 clean
	${MAKE} -s -Cexternal/json clean
	${MAKE} -s -Cexternal/socket clean

doc:
	ldoc -q . >/dev/null

install:
	mkdir -p include/callisto
	mkdir -p "${DESTDIR}${PREFIX}"/{bin,include,lib}
	cp -f callisto.h include/callisto
	cp -f lua-5.4/{lua.h,lualib.h,lauxlib.h,luaconf.h} \
		include/callisto
	cp -f csto "${DESTDIR}${PREFIX}"/bin
	cp -fR include/callisto "${DESTDIR}${PREFIX}"/include
	cp -f libcallisto.so "${DESTDIR}${PREFIX}"/lib

.PHONY: all clean clean-all doc install
