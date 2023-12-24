include config.mk

OBJS = csto.o callisto.o lcl.o lenviron.o lextra.o lfs.o ljson.o\
       lprocess.o util.o
LIBS = liblua.a cjson.a

all: csto libcallisto.a

csto: ${LIBS} ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS} ${LDFLAGS}
libcallisto.a: ${LIBS} ${OBJS}
	ar cr $@ ${OBJS} liblua.a

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
util.o: util.c

cjson.a: external/json/*.c
	${MAKE} -Cexternal/json
	mv -f external/json/cjson.a cjson.a

liblua.a: external/luasrc/*.c
	${MAKE} -Cexternal/luasrc
	mv -f external/luasrc/liblua.a .

clean:
	rm -f csto libcallisto.a ${OBJS} ${LIBS}
	rm -fr include doc/*.html doc/modules
clean-all: clean
	${MAKE} -s -Cexternal/luasrc clean
	${MAKE} -s -Cexternal/json clean

doc:
	ldoc -s . -q . >/dev/null

install:
	mkdir -p include/callisto
	mkdir -p "${DESTDIR}${PREFIX}"/{bin,include,lib}
	cp -f callisto.h include/callisto
	cp -f external/luasrc/{lua.h,lualib.h,lauxlib.h,luaconf.h} include/callisto
	cp -f csto "${DESTDIR}${PREFIX}"/bin
	cp -fR include/callisto "${DESTDIR}${PREFIX}"/include
	cp -f libcallisto.a "${DESTDIR}${PREFIX}"/lib

.PHONY: all clean clean-all doc install
