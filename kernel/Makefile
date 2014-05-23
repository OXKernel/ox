##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for kernel 
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I.. -I../include -I../include/ox -I../include/sys -I../include/platform
#CFLAGS  =	$(IFLAGS) -nostdinc -Wall -O -fstrength-reduce -fomit-frame-pointer -m32 -fpic
CFLAGS  =	$(IFLAGS) -nostdinc -Wall -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer
PLATFORM=	./platform
SYSCALL =	./syscall
.c.s:
	$(CC)	$(CFLAGS) \
	-nostdinc -I$(IFLAGS) -S -o $*.s $<

.s.o:
	$(AS)	-f elf -o $*.o $<


OBJS = \
	lowcore.o \
	syscall.o	\
	init.o      \
	exit.o		\
	exec.o      \
	fork.o		\
	mktime.o	\
	misc.o      \
	ox_main.o	\
	panic.o		\
	process_queue.o \
	ptrace.o	\
	scheduler.o	\
	signal.o	\
	def_int.o	\
	syscall_tab.o

AOBJS = \
	lowcore.a \
	syscall.a	\
	init.o		\
	exit.o		\
	exec.o      \
	fork.o		\
	mktime.o	\
	misc.o      \
	ox_main.o	\
	panic.o		\
	process_queue.o \
	ptrace.o	\
	scheduler.o	\
	signal.o	\
	def_int.o	\
	syscall_tab.o

kernel.o: $(OBJS)
	  $(LD) -r -melf_i386 -o kernel.o $(OBJS)
	  ar rv kernel.a $(AOBJS)
	  sync

lowcore.o:
	-cd $(PLATFORM); make

syscall.o:
	-cd $(SYSCALL);  make

clean:
	-rm -f core *.o *.a 
	-cd $(PLATFORM); make clean
	-cd $(SYSCALL);  make clean
