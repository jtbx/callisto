# luasocket src/makefile
#
# Definitions in this section can be overriden on the command line or in the
# environment.

# LUAV: 5.1 5.2 5.3 5.4
# lua version to build against
LUAV?=5.4

# MYCFLAGS: to be set by user if needed
MYCFLAGS?=

# MYLDFLAGS: to be set by user if needed
MYLDFLAGS?=

# DEBUG: NODEBUG DEBUG
# debug mode causes luasocket to collect and returns timing information useful
# for testing and debugging luasocket itself
DEBUG?=NODEBUG

# DESTDIR: (no default)
# used by package managers to install into a temporary destination
DESTDIR?=

#------
# Definitions below can be overridden on the make command line, but
# shouldn't have to be.

#------
# Compiler and linker settings
AR     = ar rcu
CC     = cc
DEF    = -DLUASOCKET_${DEBUG}
INCS   = -I../../../lua-5.4
CFLAGS = ${INCS} ${DEF} \
		  -Wall -Wshadow -Wextra -Wimplicit -O2 -fpic

.SUFFIXES: .obj

.c.obj:
	${CC} ${CFLAGS} //Fo"$@" //c $<

#------
# Output file names
#
SOCKET_V=3.0.0
MIME_V=1.0.3
SOCKET_A=socket.a

#------
# Modules belonging to socket-core
#
SOCKET_OBJS= \
	luasocket.o \
	timeout.o \
	buffer.o \
	io.o \
	auxiliar.o \
	compat.o \
	options.o \
	inet.o \
	usocket.o \
	except.o \
	select.o \
	tcp.o \
	udp.o

#------
# Modules belonging mime-core
#
MIME_OBJS= \
	mime.o \
	compat.o

#------
# Modules belonging unix (local domain sockets)
#
UNIX_OBJS=\
	buffer.o \
	auxiliar.o \
	options.o \
	timeout.o \
	io.o \
	usocket.o \
	unixstream.o \
	unixdgram.o \
	compat.o \
	unix.o

#------
# Modules belonging to serial (device streams)
#
SERIAL_OBJS=\
	buffer.o \
	compat.o \
	auxiliar.o \
	options.o \
	timeout.o \
	io.o \
	usocket.o \
	serial.o

all: ${SOCKET_A}

${SOCKET_A}: ${SOCKET_OBJS} ${MIME_OBJS} ${UNIX_OBJS} ${SERIAL_OBJS}
	${AR} $@ ${SOCKET_OBJS} ${MIME_OBJS} ${UNIX_OBJS} ${SERIAL_OBJS}

clean:
	rm -f ${SOCKET_OBJS} ${SERIAL_OBJS} ${MIME_OBJS} ${UNIX_OBJS}

.PHONY: all clean

#------
# List of dependencies
#
compat.o: compat.c compat.h
auxiliar.o: auxiliar.c auxiliar.h
buffer.o: buffer.c buffer.h io.h timeout.h
except.o: except.c except.h
inet.o: inet.c inet.h socket.h io.h timeout.h usocket.h
io.o: io.c io.h timeout.h
luasocket.o: luasocket.c luasocket.h auxiliar.h except.h \
	timeout.h buffer.h io.h inet.h socket.h usocket.h tcp.h \
	udp.h select.h
mime.o: mime.c mime.h
options.o: options.c auxiliar.h options.h socket.h io.h \
	timeout.h usocket.h inet.h
select.o: select.c socket.h io.h timeout.h usocket.h select.h
serial.o: serial.c auxiliar.h socket.h io.h timeout.h usocket.h \
  options.h unix.h buffer.h
tcp.o: tcp.c auxiliar.h socket.h io.h timeout.h usocket.h \
	inet.h options.h tcp.h buffer.h
timeout.o: timeout.c auxiliar.h timeout.h
udp.o: udp.c auxiliar.h socket.h io.h timeout.h usocket.h \
	inet.h options.h udp.h
unix.o: unix.c auxiliar.h socket.h io.h timeout.h usocket.h \
	options.h unix.h buffer.h
usocket.o: usocket.c socket.h io.h timeout.h
wsocket.o: wsocket.c socket.h io.h timeout.h usocket.h
