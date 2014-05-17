
ALL:
	#gcc -D_TEST_PIO_UTIL -m32 -fpic -c pioutil.c
	gcc -D_TEST_PIO_UTIL -m32 -c pioutil.c
	nasm -f elf pioread.s
	nasm -f elf piowrite.s
	gcc -m32 -fpic pioutil.o pioread.o piowrite.o -o pioutil

test: 
	gcc -fomit-frame-pointer -m32 -masm=intel -S pioutil.c
	nasm -f elf test.s
	nasm -f elf pioread.s
	nasm -f elf piowrite.s
	gcc -m32 piotest.c test.o -o util
	gcc -D_TEST_PIO_UTIL -m32 pioutil.c pioread.o piowrite.o -o testutil 
	gcc -m32 -masm=intel -S piotest.c

clean:
	rm -f *.o testutil pioutil util
