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
#include <sys/types.h>
#include <sys/stat.h>

int
main(int argc, char **argv)
{
    FILE *img = NULL;
    FILE *out = NULL;
    int size  = 0;
    int ch    = 0;
    int osize = 1474560;
    struct stat stbuf = {0};

    if(argc != 3) {
        fprintf(stderr,"syntax:: mkboot image outimage\n");
        return 1;
    }

    img = fopen(argv[1], "r");
    out = fopen(argv[2], "w");

    if(!img) {
        fprintf(stderr,"error opening image file [%s]\n", argv[1]);
        return 2;
    }

    if(!out) {
        fprintf(stderr,"error opening output file [%s]\n", argv[2]);
        return 2;
    }

    stat(argv[1], &stbuf);
    size = stbuf.st_size;

    while(size--) {
        ch = fgetc(img);
        fputc(ch, out);
    }

    size = osize - stbuf.st_size;
    ch = 0x0;
    while(size--) {
        fputc(ch, out);
    }
    fclose(img);
    fclose(out);
    return 0;
}/* main */
