##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
##########################################################
# makefile for drivers
#
##########################################################

AS	=	/usr/bin/nasm
CC	=	/usr/bin/cc
LD	=	/usr/bin/ld

LDFLAGS =	-s -x -M
IFLAGS  =       -I. -I.. -I../include -I../../../../include -I../../../../include/platform \
		-I../include/ox -I../include/sys -I../include/platform
#CFLAGS  =	$(IFLAGS) -nostdinc -Wall -O -fstrength-reduce -fomit-frame-pointer -m32 -fpic
CFLAGS  =	$(IFLAGS) -nostdinc -Wall -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer
CHAR    = 	./chara
BLOCK   = 	./block

.c.s:
	$(CC)	$(CFLAGS) \
	-nostdinc -I$(IFLAGS) -S -o $*.s $<
.s.o:
	$(AS)	-f elf -o $*.o $<

#
# This should be generated with a configuration
# script when we are able to support various
# hardware configurations.
#
OBJS = \
	$(CHAR)/keybisr.o \
	$(CHAR)/keyboard.o  \
	$(CHAR)/rs_io.o     \
	$(CHAR)/console.o   \
	$(CHAR)/serial.o    \
	$(CHAR)/tty_io.o    \
	$(CHAR)/pit.o \
	$(CHAR)/delay.o \
	$(BLOCK)/pio/pioread.o \
	$(BLOCK)/pio/piowrite.o \
	$(BLOCK)/pio/pioutil.o \
	$(BLOCK)/ide/ide.o

drivers.o: $(OBJS)
	  $(LD) -r -melf_i386 -o drivers.o $(OBJS)
	  ar rv drivers.a $(OBJS)
	  sync

clean:
	-rm -f core *.o *.a 
	-rm -f $(CHAR)/core  $(CHAR)/*.o   $(CHAR)/*.a
	-rm -f $(BLOCK)/core $(BLOCK)/*.o $(BLOCK)/*.a
	-rm -f $(BLOCK)/pio/*.o
