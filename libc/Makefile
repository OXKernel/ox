##########################################################
# Copyright (C)  Roger George Doss. All Rights Reserved.
##########################################################
#
# makefile for libc
#
##########################################################
AR       =  ar
ARFLAGS  =  rcs
STD      =  std
SYSCALL  =  syscall

lic.a:
	-cd $(STD);     make
	-cd $(SYSCALL); make
	$(AR) $(ARFLAGS) libc.a $(STD)/*.o $(SYSCALL)/*.o

clean:
	-cd $(STD);     make clean
	-cd $(SYSCALL); make clean
	-rm libc.a

depend:
	-cd $(STD);     make depend
	-cd $(SYSCALL); make depend
