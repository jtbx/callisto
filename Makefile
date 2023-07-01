PREFIX = /usr/local

CC       = cc
CFLAGS   = -std=c99 -pedantic -fpic -O2 -Wall -Wextra -Wno-override-init -I. -Ilua-5.4
CPPFLAGS = -D_DEFAULT_SOURCE -DLUA_USE_READLINE
LDFLAGS = -lm -lreadline

OBJS = csto.o lcallisto.o lcl.o lenviron.o lextra.o lfile.o \
	   ljson.o lmath.o los.o lprocess.o lsocket.o util.o
LIBS = liblua.a cjson.a socket.a

all: csto libcallisto.so

csto: ${LIBS} ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS} ${LDFLAGS}
libcallisto.so: ${LIBS} ${OBJS}
	${CC} -shared ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}

.SUFFIXES: .o

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c $<

csto.o: csto.c lcallisto.h
lcallisto.o: lcallisto.c lcallisto.h
lcl.o: lcl.c lcallisto.h
lextra.o: lextra.c lcallisto.h
lenviron.o: lenviron.c lcallisto.h
lfile.o: lfile.c lcallisto.h
ljson.o: ljson.c lcallisto.h
lmath.o: lmath.c lcallisto.h
los.o: los.c lcallisto.h
lprocess.o: lprocess.c lcallisto.h
lsocket.o: lsocket.c lcallisto.h
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
	rm -fr doc/*.html doc/modules
	${MAKE} -s -Clua-5.4 clean
	${MAKE} -s -Cexternal/json clean
	${MAKE} -s -Cexternal/socket clean

doc:
	ldoc -q . >/dev/null

install:
	mkdir -p ${DESTDIR}${PREFIX}/{bin,lib}
	chmod +x csto libcallisto.so
	cp -f csto ${DESTDIR}${PREFIX}/bin

.PHONY: all clean doc install
