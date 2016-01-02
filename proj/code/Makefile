# Makefile for the project

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	Main.c
SRCS+=	Assembly.S Bitmap.c Game.c Graphics.c Keyboard.c Mouse.c Position.c Timer.c Utilities.c VBE.c
SRCS+=  WallyNIX.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=
BINDIR? = /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>