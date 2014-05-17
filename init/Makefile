##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for init
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =   -I. -I.. -I../include -I../include/ox -I../include/sys -I../include/platform
#CFLAGS  =	$(IFLAGS) -Wall -O -nostdinc -fstrength-reduce -fomit-frame-pointer -m32 -fpic
CFLAGS  =	$(IFLAGS) -Wall -nostdinc -m32 -nostdlib -nostartfiles -nodefaultlibs

.s.o:
	$(AS)	-f elf -o $*.o $<

OBJS = \
	init.o

init:   $(OBJS)
	$(CC) -static -m32 -o init $(OBJS) -nostdinc -nostdlib -nostartfiles -nodefaultlibs

clean:
	-rm -f core *.o *.a
	-rm init
