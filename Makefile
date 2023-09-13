# See LICENSE file for copyright and license details.

VERSION = post-2.0.2

PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

CPPFLAGS  = -DVERSION=\"${VERSION}\"
CFLAGS    = -std=c99 -pthread -pedantic -Wall -Wextra \
	    -Wno-unused-parameter -O3 ${CPPFLAGS}
DBGFLAGS  = -std=c99 -pthread -pedantic -Wall -Wextra \
            -Wno-unused-parameter -Wno-unused-function -g3 \
	    -fsanitize=address -fsanitize=undefined -${CPPFLAGS}

CC = cc

all: nissy

nissy: clean
	${CC} ${CFLAGS} -o nissy src/*.c

nissy.exe:
	x86_64-w64-mingw32-gcc ${CFLAGS} -static -o nissy.exe src/*.c

debug:
	${CC} ${DBGFLAGS} -o nissy src/*.c

test:
	${CC} ${DBGFLAGS} -DTEST -o test src/*.c tests/*.c

clean:
	rm -rf nissy nissy.exe

.PHONY: all debug test clean
