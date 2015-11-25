# Makefile for the project

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	Main.c
SRCS+=	Assembly.S Graphics.c Keyboard.c Mouse.c Timer.c Utilities.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>