##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for asm_core, the kernel's lowest level
# primitives, written in assembler
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I.. -I../include -I../include/ox -I../include/sys -I../include/platform
#CFLAGS  =	$(IFLAGS) -nostdinc -Wall -O -fstrength-reduce -fomit-frame-pointer -fpic
CFLAGS  =	$(IFLAGS) -nostdinc -Wall -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer

.c.s:
	$(CC)	$(CFLAGS) \
	-nostdinc -I$(IFLAGS) -S -o $*.s $<

.s.o:
	$(AS)	-f elf -o $*.o $<


OBJS = \
	atom.o	    \
	scheduler.o \
	exception.o \
	interrupt.o \
	io.o        \
	start.o     \
	syscall.o   \
	util.o

asm.o:  $(OBJS)
	$(LD) -r -melf_i386 -o asm.o $(OBJS)
	ar rv asm.a $(OBJS)
	mv asm.o ..
	mv asm.a ..
	sync

clean:
	-rm -f core *.o *.a 
