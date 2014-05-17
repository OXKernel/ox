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
 *      malloc.h
 *
 * @description:
 *      user and kernel memory allocator interface. Allocates memory
 *      in PAGE_SIZE pages and redistributes the allocation
 *      to be more efficient to the caller. Calls page_alloc/free
 *      for actual memory allocation and maintains system
 *      level tables for allocation.
 *
 * @author:
 *      Dr. Roger G. Doss, PhD
 *
 */
#ifndef _MALLOC_H
#define _MALLOC_H

void *malloc(unsigned int size);
void free(void *addr);

void *balloc(unsigned int size);
void bfree(void *addr);

void *kmalloc(unsigned int size);
void kfree(void *addr);

void kmalloc_unit_test();
void malloc_unit_test();

int alloc_gdt();
void setup_paging();
unsigned *get_page_dir();

#endif
