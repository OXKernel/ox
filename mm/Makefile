##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for memory manager
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I../include -I../include/ox -I../include/sys -I../include/platform -I../include/mm
#CFLAGS  =	$(IFLAGS) -nostdinc -Wall -O -fstrength-reduce -fomit-frame-pointer -m32 -fpic
CFLAGS  =	$(IFLAGS) -nostdinc -Wall -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -D_ENABLE_PAGING

.s.o:
	$(AS) -f elf -o $*.o $<

OBJS = \
	page.o \
	page_enable.o \
	dma.o \
	kmalloc.o \
	malloc.o

mm.o:	$(OBJS)
	$(LD) -r -melf_i386 -o mm.o $(OBJS)
	ar rv mm.a $(OBJS)
	sync

clean:
	-rm -f core *.o *.a
