##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
# makefile for syscall
#
##########################################################
AR       =  ar
AS       =  as
CC       =  gcc
LD       =  ld
ARFLAGS  =  rcs
LDFLAGS  =  -s -x
IFLAGS   =  -nostdinc -I../../include
CFLAGS	 =  $(IFLAGS) -D_POSIX_SOURCE -Wall -O -fstrength-reduce \
	    -fomit-frame-pointer -finline-functions -m32 -fpic
CPP	 =  cc -E $(IFLAGS)

.c.s:  $(CC) $(CFLAGS)  -S -o $*.s $<
.s.o:  $(AS) -c -o $*.o $<
.c.o:  $(CC) $(CFLAGS) -c -o $*.o $<

OBJS = \
	string.o \
	strerror.o

all:   $(OBJS)

clean:
	rm -f core *.o *.a tmp_make
	for i in *.c;do rm -f `basename $$i .c`.s;done

depend:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do echo -n `echo $$i | sed 's,\.c,\.s,'`" "; \
		$(CPP) -M $$i;done) >> tmp_make
	cp tmp_make Makefile

### Dependencies:
