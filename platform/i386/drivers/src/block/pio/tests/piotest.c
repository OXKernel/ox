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
/*
 * get_drive_head:
 *
 * drive can be 0 or 1 for first or second drive.
 * head can be 0-15 values of the last 4 bits.
 *
 */
unsigned char get_drive_head(unsigned char drive, 
                             unsigned char head)
{
   unsigned char val = 160; // bit 7 == 1, bit 5 == 1, all others 0.
   val |= (drive << 4); // Set the forth bit.
   val |= (head & 240); // Set bottom for bits 0-3.
}

unsigned char read_disk(unsigned char drive_head, 
               unsigned char sector,
               unsigned char cylinder1,
               unsigned char cylinder2)
{
    unsigned char tmp = drive_head;
    tmp += sector;
    tmp += cylinder1;
    tmp += cylinder2;
    return tmp;
}

int foo(unsigned char f)
{
        return f;
}

extern int test(unsigned char foo);
extern int test1(unsigned char foo);
extern int test2(unsigned char foo);
extern int pio_read(unsigned char drive_head, unsigned char sector,
                    unsigned char cyl1, unsigned char cyl2, char *buff);

int status_check(int operation, unsigned char error_reg, unsigned char status_reg)
{
    // Bit 0 is the error bit, if it is set
    // and Bit 7 (2^7)=128=0x80=BSY is 0 then
    // there was an error.
    int rtvl = 0;
    if((status_reg & 0x1) && !(status_reg & 0x80)) {
        if(operation == 0) {
            printf("error reading drive\n");
        } else if(operation == 1) {
            printf("error writing drive\n");
        }
        rtvl = 1;
        // Look into error_reg to output
        // what error it was, and return error.
        // EIDE has bits 5,3 MC Medium changed, MCR Medium change required.
        if(error_reg & 128) {
            printf("BBK(bit 7) sector marked as bad by host\n");
        }
        if(error_reg & 64) {
            printf("UNC(bit 6) uncorrectable data error\n");
        }
        if(error_reg & 16) {
            printf("NID(bit 4) ID mark not found\n");
        }
        if(error_reg & 4) {
            printf("ABT(bit 2) command abort\n"); 
        }
        if(error_reg & 2) {
            printf("NTO(bit 1) track 0 not found\n");
        }
        if(error_reg & 1) {
            printf("NDM(bit 0) data address mark not found\n");
        }
        // TODO:= Panic the kernel here.
        // Or, return the error to the caller and then panic
        // the kernel of writing out the CHS information.
        // - Integrate into the assembler.
    }
    return rtvl;
}

extern int bit_test(unsigned char reg);

int
main()
{
    unsigned char status_reg = 0, error_reg = 1;
    char ptr[512]={0};
    printf("%x\n",get_drive_head(0,0));
    printf("%x\n",test(0xAB));
    printf("%x\n",test1(0xBA));
    printf("%x\n",test2(0xCA));
    printf("ptr=%x %x\n",ptr,pio_read(0xA,0xB,0xC,0xD,ptr));
    printf("%x\n",0xC+0x4);
    printf("%x\n",0x10+0x4);
    status_reg = 1;
    // status_reg |= (1<<7); // Set BSY
    if(status_check(0, error_reg, status_reg) == 0) {
        printf("status_check:: no error\n");
    }
    if(bit_test(status_reg)==1) {
        printf("bit_test:: error\n");
    } else {
        printf("bit_test:: no error\n"); 
    }
    return 0;
}
