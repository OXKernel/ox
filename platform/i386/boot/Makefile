###################################################################################
# Copyright (C) Roger G. Doss. All Rights Reserved.
####################################################################################

#
# stage 1 and stage 2 loaders
SRC=s1.s s2.s
OBJS=s1.o s2.o
TDIR=../../..
AS=nasm

all: $(OBJS) mkboot
	@if [ -e "$(TDIR)/vmox" ]; then \
		cat s1 >  	$(TDIR)/vmox.boot; 	\
		cat s2 >> 	$(TDIR)/vmox.boot;	\
		cat $(TDIR)/vmox >> 	$(TDIR)/vmox.boot;	\
		exit; \
	fi; \
	echo "error while making vmox.boot, file ../vmox not found"

test_install: $(OBJS) test
	@if [ -e "tests/vmox" ]; then \
		cat s1 >  ./boot; \
		cat s2 >> ./boot; \
		cat tests/vmox >> ./boot; \
		dd if=./boot of=/dev/fd0 bs=512; \
		exit; \
	fi; \
	echo "error during installation type make"

test_kernel: $(OBJS) test
	@if [ -e "tests/vmox" ]; then \
		cat s1 >  ./boot; \
		cat s2 >> ./boot; \
		cat tests/vmox >> ./boot; \
		mv ./boot vmox; \
		./mkboot vmox vmox.img; \
		exit; \
	fi; \
	echo "error during installation type make"

test_bin_kernel: $(OBJS) test_bin bin
	@if [ -e "tests/vmox" ]; then \
		cat s1 >  ./boot; \
		cat s2 >> ./boot; \
		cat tests/vmox.bin >> ./boot; \
		mv ./boot vmox; \
		./mkboot vmox vmox.img; \
		exit; \
	fi; \
	echo "error during installation type make"

clean:
	rm -f s1 s2
	rm -f tests/vmox
	rm -f tests/vmox.bin
	rm -f tests/vmox.o
	rm -f elf/elftest
	rm -f elf/check_null
	rm -f elf/get_data
	rm -f core boot
	rm -f s1_list s2_list s1.h s2.h

test:
	cc -m32 mkboot.c -o mkboot
	cd tests; cc -c -m32 -fpic vmox.c; ld -melf_i386 -T linker.ld vmox.o -o vmox # -Ttext 0x100000
	# cd tests; cc -m32 -fPIE -pie vmox.c -o vmox -nostdinc -nostdlib -static -Ttext 0x100000
	cd elf; cc elftest.c -o elftest
	cd elf; cc check_null.c -o check_null
	cd elf; cc get_data.c -o get_data

s1: s1.s
	$(AS) $(BUILD_FLAGS) s1.s
	
s2: s2.s
	$(AS) $(BUILD_FLAGS) s2.s

s1.o: s1.s
	$(AS) $(BUILD_FLAGS) s1.s

s2.o: s2.s
	$(AS) $(BUILD_FLAGS) s2.s

s1ls:
	$(AS) -l s1_list s1.s
	
s2ls:
	$(AS) -l s2_list s2.s

s1h:
	$(AS) -e s1.s > s1.h

s2h:
	$(AS) -e s2.s > s2.h

bin:
	objcopy ./tests/vmox ./tests/vmox.bin --target=binary \
	--input-target=elf32-i386	

test_bin:
	cc -m32 mkboot.c -o mkboot
	#cd tests; cc -c -m32 vmox.c; ld -T mlinker.ld -melf_i386 vmox.o -o vmox
	cd tests; cc -c -m32 vmox.c; ld -melf_i386 vmox.o -o vmox -Ttext 0x14000
	cd elf; cc elftest.c -o elftest
	cd elf; cc check_null.c -o check_null
	cd elf; cc get_data.c -o get_data

mkboot:
	cc mkboot.c -o mkboot	
