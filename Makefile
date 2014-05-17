#
# Copyright (C) Roger George Doss. All Rights Reserved.
#
#
#
# top-level makefile for OX 
#
#

AS      =       /usr/bin/nasm
CC      =       /usr/bin/cc
LD      =       /usr/bin/ld

LDFLAGS =       -s -x -M
IFLAGS  =       -I. -I.. -I../include -I../include/ox -I../include/sys -I../include/platform
#CFLAGS  =       $(IFLAGS) -Wall -nostdinc -O -fstrength-reduce -fomit-frame-pointer -fpic
CFLAGS  =       $(IFLAGS) -Wall -nostdinc -m32 -fpic

.s.o:
	$(AS)  -f elf -o $*.o $<

LOBJS = \
 	./kernel/kernel.o     \
 	./drivers/drivers.o   \
 	./fs/fs.o     	      \
 	./mm/mm.o	      \
	./libk/libk.o

OBJS = \
	./libk/util.o \
	./libk/strtoul.o \
	./libk/vsprintk.o \
	./libk/string.o \
	./libk/bit.o \
	./libk/errno.o \
	./libk/printk.o \
	./libk/ultostr.o \
	./mm/page.o \
	./mm/page_enable.o \
	./mm/malloc.o \
	./mm/dma.o \
	./mm/kmalloc.o \
	./fs/file.o \
	./fs/link.o \
	./fs/krealpath.o \
	./fs/bitmap.o \
	./fs/inode.o \
	./fs/dev.o \
	./fs/paths.o \
	./fs/block.o \
	./fs/dir.o \
	./fs/init.o \
	./platform/i386/drivers/src/chara/rs_io.o \
	./platform/i386/drivers/src/chara/keybisr.o \
	./platform/i386/drivers/src/chara/keyboard.o \
	./platform/i386/drivers/src/chara/tty_io.o \
	./platform/i386/drivers/src/chara/serial.o \
	./platform/i386/drivers/src/chara/console.o \
	./platform/i386/drivers/src/chara/pit.o \
	./platform/i386/drivers/src/chara/delay.o \
	./platform/i386/drivers/src/block/pio/pioread.o \
	./platform/i386/drivers/src/block/pio/piowrite.o \
	./platform/i386/drivers/src/block/pio/pioutil.o \
	./platform/i386/drivers/src/block/ide/ide.o \
	./platform/i386/arch/interrupt.o \
	./platform/i386/arch/exception.o \
	./platform/i386/arch/segment.o \
	./platform/i386/arch/idt.o \
	./platform/i386/arch/protected_mode.o \
	./platform/i386/arch/main.o \
	./platform/i386/arch/asm_core/interrupt.o \
	./platform/i386/arch/asm_core/syscall.o \
	./platform/i386/arch/asm_core/exception.o \
	./platform/i386/arch/asm_core/util.o \
	./platform/i386/arch/asm_core/scheduler.o \
	./platform/i386/arch/asm_core/io.o \
	./platform/i386/arch/asm_core/atom.o \
	./platform/i386/arch/asm_core/start.o \
	./platform/i386/arch/io_req.o \
	./kernel/signal.o \
	./kernel/ox_main.o \
	./kernel/exit.o \
	./kernel/fork.o \
	./kernel/init.o \
	./kernel/exec.o \
	./kernel/syscall_tab.o \
	./kernel/scheduler.o \
	./kernel/def_int.o \
	./kernel/mktime.o \
	./kernel/ptrace.o \
	./kernel/misc.o \
	./kernel/panic.o \
	./kernel/process_queue.o \
	./kernel/syscall/syscall_0.o \
	./kernel/syscall/syscall_1.o \
	./kernel/syscall/syscall_2.o \
	./kernel/syscall/syscall_3.o \
	./kernel/syscall/syscall_4.o \
	./kernel/syscall/syscall_5.o

ARCHIVES= \
	./kernel/kernel.a     \
	./drivers/drivers.a   \
	./fs/fs.a     	      \
	./mm/mm.a	      \
	./libk/libk.a

all:	$(LOBJS)
	#$(CC) -m32 -nostdinc -nostdlib -nostartfiles -nodefaultlibs -static $(TOBJS) -o vmox -Ttext 0x100000 --unresolved-symbols=ignore-all
	#$(LD) -melf_i386 -Ttext 0x100000 -Tdata 0x200000 $(OBJS) -o vmox
	#$(LD) -melf_i386 -Ttext 0x14000 $(OBJS) -o vmox --unresolved-symbols=ignore-all
	#$(LD) -melf_i386 -T ./ld/linker.ld $(TOBJS) -o vmox --unresolved-symbols=ignore-all
	$(CC) -m32 -nostdinc -nostdlib -nostartfiles -nodefaultlibs -static $(OBJS) -o vmox -Ttext 0x100000 # --unresolved-symbols=ignore-all
	strip vmox
	#$(LD) -melf_i386 -T nlinker.ld $(TOBJS) -o vmox --unresolved-symbols=ignore-all
	#strip vmox
	#$(LD) -melf_i386 -T linker.ld $(OBJS) -o vmox
	#$(CC) -m32 -nostdinc -nostdlib -nostartfiles -nodefaultlibs -static ./kernel/platform/main.o $(ARCHIVES) ./kernel/lowcore.a ./kernel/platform/asm.a ./drivers/chara/console.o ./kernel/platform/asm_core/io.o -o vmox -Ttext 0x100000
	#$(CC) -m32 -nostdinc -nostdlib -nostartfiles -nodefaultlibs -static $(OBJS) -o vmox -Ttext 0x100000
	#$(CC) -m32 -fPIE -pie -nostdinc -nostdlib -nostartfiles -nodefaultlibs -static vmox.o -o vmox -Ttext 0x100000
	#$(LD) -melf_i386 $(OBJS) -o vmox -Ttext 0x100000 # 0xe9300
	#$(CC) -m32 -nostdinc -nostdlib vmox.o -o vmox -Ttext 0x100000
	cd libc; make
	cd boot; make
	cd init; make
	./boot/mkboot vmox.boot vmox.img

bin_kernel:	$(LOBJS)
	$(LD) -melf_i386 -T ./ld/flat_linker.ld $(OBJS) -o vmox # --unresolved-symbols=ignore-all
	objcopy ./vmox ./vmox.bin --target=binary --input-target=elf32-i386
	cp ./vmox ./vmox.exe
	cp ./vmox.bin ./vmox
	cd libc; make
	cd boot; make
	cd init; make
	./boot/mkboot vmox.boot vmox.img

kernel: $(OBJS)
	$(LD) -o vmox $(OBJS) -Ttext 0x100000

install_old: all
	@if [ -e "./vmox.boot" ]; then \
		dd if=./vmox.boot of=/dev/fd0 bs=512; \
		exit; \
	fi; \
	echo "error during installation"

install: all
	@if [ -e "./vmox.boot" ]; then \
		dd if=./vmox.boot of=/dev/sdb bs=512; \
		exit; \
	fi; \
	echo "error during installation"

libc:
	cd libc; make

boot:
	cd boot; make

init:
	cd init; make

# build for kernel objects
./kernel/kernel.o:
	cd kernel; make

./drivers/drivers.o:
	cd drivers; make

./fs/fs.o:
	cd fs;     make

./mm/mm.o:
	cd mm;     make

./libk/libk.o:
	cd libk;   make

# clean all
clean_all:
	cd kernel;  make clean
	cd drivers; make clean
	cd fs;      make clean
	cd mm;      make clean
	cd libk;    make clean
	cd libc;    make clean
	cd boot;    make clean
	cd init;    make clean
	rm -f vmox
	rm -f vmox.boot
	rm -f vmox.img

kernel_clean:
	cd kernel;  make clean
	cd drivers; make clean
	cd fs;      make clean
	cd mm;      make clean
	cd libk;    make clean
	rm -f vmox
	rm -f vmox.boot
	rm -f vmox.img

libc_clean:
	cd libc; make clean
	
boot_clean:
	cd boot; make clean
	
init_clean:
	cd init; make clean
	
clean:
	@echo "targets are: clean_all kernel_clean libc_clean boot_clean init_clean"

bin:
	objcopy ./vmox ./vmox.bin --target=binary \
	--input-target=elf32-i386	
	mv ./vmox.bin ./vmox

#
# EOF
#
