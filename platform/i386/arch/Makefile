##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for lowcore.o, the kernel's low-level
# machine-dependent primives
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I../include -I../.. -I../../../include -I../../include/ox \
		-I../../include/sys -I../../include/platform
#CFLAGS  =	$(IFLAGS) -nostdinc -Wall -O -fstrength-reduce -fomit-frame-pointer -D_VM_DISABLED -m32 -fpic
CFLAGS  =	$(IFLAGS) -nostdinc -Wall -m32 -D_VM_DISABLED -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -D_USE_NEW_OFFSET
ASMCORE = 	./asm_core

.c.s:
	$(CC)	$(CFLAGS) \
	-nostdinc -I$(IFLAGS) -S -o $*.s $<

.s.o:
	$(AS)	-f elf -o $*.o $<


OBJS = \
	asm.o            \
	gdt_init.o \
	exception.o	 \
	interrupt.o      \
	protected_mode.o \
	segment.o	 \
	io_req.o	 \
	main.o

AOBJS = \
	asm.a            \
	gdt_init.o \
	exception.o	 \
	interrupt.o      \
	protected_mode.o \
	segment.o	 \
	io_req.o	 \
	main.o

lowcore.o: $(OBJS)
	  $(LD) -r -melf_i386 -o lowcore.o $(OBJS)
	  ar rv lowcore.a $(AOBJS)
	  mv lowcore.o ../../../kernel
	  mv lowcore.a ../../../kernel
	  sync

asm.o:
	-cd $(ASMCORE); make

clean:
	-rm -f core *.o *.a 
	-rm -f ../../kernel/lowcore.o ../../kernel/lowcore.a
	-cd $(ASMCORE); make clean
