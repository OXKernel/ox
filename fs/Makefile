##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for filesystem
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I.. -I../include -I../include/ox -I../include/sys -I../include/platform -I../include/ox/fs
#CFLAGS  =	$(IFLAGS) -Wall -O -nostdinc -fstrength-reduce -fomit-frame-pointer -m32 -fpic
CFLAGS  =	$(IFLAGS) -Wall -nostdinc -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer

.c.s:
	$(CC)	$(CFLAGS) \
	-nostdinc -I$(IFLAGS) -S -o $*.s $<

.s.o:
	$(AS)	-f elf -o $*.o $<

OBJS = \
	paths.o \
	bitmap.o \
	dev.o \
	block.o \
	krealpath.o \
	inode.o \
	file.o \
	dir.o \
	link.o \
	init.o

fs.o:   $(OBJS)
	$(LD) -r -melf_i386 -o fs.o $(OBJS)
	ar rv fs.a $(OBJS)
	sync

clean:
	-rm -f core *.o *.a

