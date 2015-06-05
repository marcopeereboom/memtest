# $memtest: Makefile,v 1.2 2010/03/03 23:41:33 marco Exp $

PROG=memtest
NOMAN=

SRCS= memtest.c
COPT+= -O2
DEBUG+= -ggdb3 
CFLAGS+= -Wall
LDFLAGS+=-lm

.include <bsd.prog.mk>
