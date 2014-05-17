##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for system calls. The following produces all
# the high-level system call routines, all of which
# have the 'sys_' prefix, and are organized in
# source files that reflect the number of parameters
# that the system call requires.
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I.. -I../../include
#CFLAGS  =	$(IFLAGS) -nostdinc -Wall -O -fstrength-reduce -fomit-frame-pointer -D_POSIX_SOURCE -m32 -fpic
CFLAGS  =	$(IFLAGS) -nostdinc -Wall -D_POSIX_SOURCE -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer

.c.s:
	$(CC)	$(CFLAGS) \
	-nostdinc -I$(IFLAGS) -S -o $*.s $<

.s.o:
	$(AS)	-f elf -o $*.o $<


OBJS = \
	syscall_0.o   \
	syscall_1.o   \
	syscall_2.o   \
	syscall_3.o   \
	syscall_4.o   \
	syscall_5.o
	
syscall.o: $(OBJS)
	  $(LD) -r -melf_i386 -o syscall.o $(OBJS)
	  ar rv syscall.a $(OBJS)
	  mv syscall.o ..
	  mv syscall.a ..
	  sync

clean:
	-rm -f core *.o *.a 
