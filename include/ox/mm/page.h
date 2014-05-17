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
 * @file:
 *      page.h
 *
 * @description:
 *      kernel page memory allocator maintains a
 *      set of bitmaps representing each possible free
 *      page and allocates by selecting a range
 *      of pages that are free. Uses a first fit strategy.
 *      Divides memory into user and kernel, provides
 *      two page allocators user and kernel, provides
 *      facility to set/unset pages as read only.
 *
 * NOTES:
 *      Some macros may need to change, for example,
 *      PT_INDEX and PD_INDEX.
 *
 * @author:
 *      Dr. Roger G. Doss, PhD
 *
 */
#ifndef _PAGE_H
#define _PAGE_H

#include <platform/protected_mode_defs.h>

#define PAGE_SIZE       4096

#define MAKE_PAGE_ADDR(address,avail,us,rw,p) ((unsigned long)((((unsigned)address)) | ((avail) << 8) | ((us) << 2) | ((rw) << 1) | (p)))

#define PAGE_PRESENT        1
#define PAGE_NOT_PRESENT    0
#define PAGE_SUPERVISOR     0
#define PAGE_USER           1
#define PAGE_READ_ONLY      0
#define PAGE_READ_WRITE     1

// User page allocator.
void *page_alloc(unsigned nr_pages);
void page_free(void *addr, unsigned nr_pages);

// Kernel page allocator.
void *kpage_alloc(unsigned nr_pages);
void kpage_free(void *addr, unsigned nr_pages);

unsigned mem_get_size();
void mem_init();

void mem_set_read_only(void *addr, unsigned nr_pages);
void mem_unset_read_only(void *addr, unsigned nr_pages);

unsigned get_nr_pages();// Highest number page that can be allocated to user.
unsigned get_nr_kpages();// Highest number page that can be allocated to kernel.
unsigned get_nr_allocated_pages();//Number of pages allocated to user.
unsigned get_nr_kallocated_pages();//Number of pages allocated to kernel.
unsigned get_user_bytes_free();//Total free bytes to user.
unsigned get_kernel_bytes_free();//Total free bytes to kernel.

/*
 * Additional macros for retrieving values from
 * page table entries.
 *
 * Use as :=
 *  printf("address = %d\n",GET_PAGE_ADDRESS(address));
 *  printf("avail = %d\n",GET_PAGE_AVAIL(address));
 *  printf("user = %d\n",GET_PAGE_USER(address));
 *  printf("read/write = %d\n",GET_PAGE_READ_WRITE(address));
 *  printf("present = %d\n",GET_PAGE_PRESENT(address));
 *
 */ 
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

#define PT_INDEX(virt) ({ \
    (((virt) >> 12) & 0x3FF); \
})

#define PTE_PRESENT     1
#define PTE_READ_WRITE  2
#define PTE_USER        4

#define PT_SET_READ_ONLY(entry) \
    (entry) &= ~2;

#define PT_SET_ATTRIB(entry, attrib) \
   (entry) |= (attrib);

#define PT_SET_FRAME(entry, addr) \
    (entry) = ((entry) & ~0xFFFFF000) | (addr);

// Page directory
#define PD_INDEX(virt) \
    (((virt) >> 22) & 0x3ff)

#define PDE_PRESENT     1
#define PDE_READ_WRITE  2
#define PDE_USER        4

#define PD_SET_ATTRIB(entry, attrib) \
    (entry) |= (attrib);

#define PD_SET_FRAME(entry, addr) \
    (entry) = ((entry) & ~0xFFFFF000) | (addr);

#endif
