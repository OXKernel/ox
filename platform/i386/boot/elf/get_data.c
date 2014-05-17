/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#define kstat   stat
#define kopen   open
#define kread   read
#define kclose close
#define printk printf
#define MAX_PATH 256

/*
 * file_load:
 *
 * load a file from disk to memory.
 * Return 1 if success.
 * Return 0 if there was an error.
 *
 */
int file_load(const char *filename, char **image, unsigned int *image_size)
{
    struct stat stbuf={0};
    int fd = -1;
    char *mem = NULL;
    if(!filename || !image || !image_size) {
        printk("file_load:: null param\n");
        return 0;
    }
    if(kstat(filename,&stbuf) == -1) {
        printk("file_load:: error stat'ing file [%s]\n",filename);
        return 0;
    }
    *image_size = stbuf.st_size;
    mem = (char *)malloc(stbuf.st_size + 1);
    if(mem == NULL) {
        printk("file_load:: error allocating memory\n");
        return 0;
    }
    fd = kopen(filename,O_RDONLY);
    if(fd == -1) {
        printk("file_load:: error loading file\n");
        free((void *)mem);
        return 0;
    }
    if(kread(fd, mem, stbuf.st_size) != stbuf.st_size) {
        printk("file_load:: error reading file [%s]\n",filename);
        free((void *)mem);
        kclose(fd);
        return 0;
    }
    mem[stbuf.st_size]='\0';
    *image = mem;
    kclose(fd);
    return 1;

}/* file_load */

int
main(int argc, char **argv)
{
    char *filename = NULL;
    char *image = NULL;
    char *ptr = NULL;
    FILE *out = NULL;
    int image_size = 0;
    int size = 0;
    int offset = 0;
    int i = 0;

    if(argc != 5) {
        fprintf(stderr,"syntax:: get_data file_name offset size output_file\n");
        return 1;
    }

    filename = argv[1];
    offset = strtoul(argv[2], NULL, 16);
    size = strtoul(argv[3], NULL, 16); // Input in HEX.

    if(!file_load(filename, &image, &image_size)) {
        printk("execve:: error loading file\n");
        return -1;
    }

    out = fopen(argv[4], "w");
    if(!out) {
        fprintf(stderr,"get_data:: error opening file [%s]\n", argv[4]);
        return 2;
    }

    ptr = &image[offset];

    for(i = 0; i < size; ++i) {
        fputc(ptr[i], out);
    }

    fclose(out);

}/* main */
