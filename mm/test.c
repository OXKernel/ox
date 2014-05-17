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
// This is just a test to see if we can
// compile this assembler.
//
// cc -m32 test.c -o t
// cc -m32 -I ../include test.c -o t

#include <ox/mm/page.h>

#define MAKE_PAGE_ADDRESS(address,avail,us,rw,p) ({ \
    unsigned addr = 0x0; \
    addr = (0xFFFFF000 & address) << 12; \
    addr; \
})

#if 0
#define GET_PAGE_ADDRESS(address) ({ \
    (((address) & (~(PAGE_SIZE-1))) >> 12); \
})

#define GET_PAGE_AVAIL(address) ({ \
    ((address >> 9) & 7); \
})

#define GET_PAGE_USER(address) ({ \
    ((address >> 2) & 1); \
})

#define GET_PAGE_READ_WRITE(address) ({ \
    ((address >> 1) & 1); \
})

#define GET_PAGE_PRESENT(address) ({ \
    (address & 1); \
})
#endif

void print_address(unsigned address)
{
    printf("address = %d\n",GET_PAGE_ADDRESS(address));
    printf("avail = %d\n",GET_PAGE_AVAIL(address));
    printf("user = %d\n",GET_PAGE_USER(address));
    printf("read/write = %d\n",GET_PAGE_READ_WRITE(address));
    printf("present = %d\n",GET_PAGE_PRESENT(address));
}

#define set_bit(address,bit) \
    (address) |= (1 << (bit));

int
main()
{
    register unsigned long cr0;
    unsigned int address, i;
    //__asm__ __volatile__("movl %0, %%cr0" :: "g" (cr0));

    address = MAKE_PAGE_ADDRESS(4096,0,PAGE_SUPERVISOR,PAGE_READ_WRITE,PAGE_PRESENT);
    printf("address = %d\n",GET_PAGE_ADDRESS(address));
    address  = MAKE_PAGE_ADDR(4096,0,PAGE_SUPERVISOR,PAGE_READ_WRITE,PAGE_PRESENT);
    address = 351928323;
    address = 16777223;
    //There are two problems := a) overflow we are putting too much into the pte
    // b) address at 4097 appears wrong at 16777223.
    // 
    // For the overflow problem, the max we can address in a page table
    // is 20 bits 1048575 (0xFFFFF000 >> 12) max is 1048576
    //address = MAKE_PAGE_ADDR(268431360,0,PAGE_SUPERVISOR,PAGE_READ_WRITE,PAGE_PRESENT);
    // Why does this not print out 4190208 is it integer overflow ???
    //address = MAKE_PAGE_ADDR(4190208,0,PAGE_SUPERVISOR,PAGE_READ_WRITE,PAGE_PRESENT);
    //address = 16777223;
    address = 4278190087;
    printf("address %u\n",address); 
    print_address(address);

    address = 0;
    for(i = 31; i >= 12; --i) {
        set_bit(address,i);
    }
    printf("address %u\n",address);
    printf("address %u\n",address >> 12);
    address = 0;
    for(i = 0; i < 20; ++i) {
        set_bit(address,i);
    }
    printf("address %u\n",address);
    //print_address(351928323);
    print_address(402653187);
    print_address(352321539);
}
