##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
# makefile for lib
# kernel library
#
##########################################################
AR       =  ar
AS       =  as
CC       =  cc
LD       =  ld
ARFLAGS  =  rcs
LDFLAGS  =  -s -x
IFLAGS   =  -nostdinc -I../include -I../include/platform/
#CFLAGS	 =  $(IFLAGS) -D_POSIX_SOURCE -Wall -O -fstrength-reduce \
#	    -fomit-frame-pointer -finline-functions -m32 -fpic
CFLAGS	 =  $(IFLAGS) -D_POSIX_SOURCE -Wall -m32 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer
CPP	 =  cc -E $(IFLAGS)

.c.s:  $(CC) $(CFLAGS)  -S -o $*.s $<
.s.o:  $(AS) -c -o $*.o $<
.c.o:  $(CC) $(CFLAGS) -c -o $*.o $<

OBJS = \
	bit.o	   \
	errno.o	   \
	string.o   \
	strtoul.o  \
	ultostr.o  \
	util.o     \
	printk.o   \
	vsprintk.o

all:   $(OBJS)
	$(LD) -r -melf_i386 -o libk.o $(OBJS)
	ar rv libk.a $(OBJS)
	sync

clean:
	rm -f core *.o *.a tmp_make
	for i in *.c;do rm -f `basename $$i .c`.s;done

depend:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile

### Dependencies:
