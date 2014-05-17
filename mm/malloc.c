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
//
// @file:
//      balloc.c
//
// @description:
//      This is a copy of kballoc.c but calls the user
//      page allocator instead of user page allocator.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
// @TODO
//
//      - Test most recent changes.
//
//      - Add a field in the structures for table and header
//        to include the actual blocks we allocate. See if this
//        would be included in our calculations. Then,
//        when we free, we supply the number of pages we want to free
//        along with the address. Also, the block allocator should
//        just ask for number of pages.
//        DONE see nr_pages and new calls to page_alloc/free.
//      - Make statics thread static based on ifdef.
//        DONE
//      - Write ifdef'd code for controlling the size and quantity of the allocations.
//        DONE
//      - Finish the balloc and work on bfree.
//        DONE
//      - Introduce a last allocated pointer to try to speed things up. (See comment under TODO)
//        DONE
//      - Maybe use is_free as a unique number instead of the integer 1 to allow us
//        to do some testing before free'ing (in event of double free).
//        DONE
//


#ifndef _TEST_MALLOC
#include <ox/error_rpt.h>
#include <ox/mm/page.h>
#include <ox/mm/malloc.h>
#include <platform/asm_core/util.h>
#include <stddef.h>
#endif

#ifdef _TEST_MALLOC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void asm_disable_interrupt() {}
void asm_enable_interrupt()  {}
#define printk printf
#endif

#define NR_PAGE_SIZE 4096

#ifdef _DEBUG_MALLOC
#   define NR_SIZES 2
#else
#   define NR_SIZES (1 << 16)
#endif

#define FREE_MEM 123456
#define USED_MEM 654321

typedef struct header {
    unsigned int slot;
    unsigned int is_free;
} header_t;

typedef struct mem {
    unsigned int size;      // user requested size
    unsigned int rsize;     // real block size
    unsigned int nr_blocks; // number of blocks available
    unsigned int nr_alloc;  // number of blocks allocated
    unsigned int nr_pages;  // number of pages requested
    void *memory;           // actual memory
} mem_t;

static mem_t zero_mem = {0};

typedef struct table {
    struct table *next;
    unsigned int nr_slots; // number of slots in use, if equal to NR_SIZES, we allocate another table node
    mem_t  sizes[NR_SIZES];
} table_t;

#ifdef _USE_THREAD_STATIC_GCC
static __thread table_t *root = NULL;
#elif _USE_THREAD_STATIC_VS
__declspec(thread) table_t *root = NULL;
#else
static table_t *root = NULL;
#endif

/*
 * block
 *
 * Allocate 4K blocks.
 *
 */
static void *block(unsigned int nr_pages)
{
#ifdef _TEST_MALLOC
    return malloc(NR_PAGE_SIZE * nr_pages);
#else
    return page_alloc(nr_pages);
#endif
}

static void block_free(void *addr, unsigned int nr_pages)
{
#ifdef _TEST_MALLOC
    free(addr);
#else
    page_free(addr, nr_pages);
#endif
}

#ifdef _DEBUG_MALLOC
#   define PLINE() printk("line=%d\n", __LINE__);
#else
#   define PLINE()
#endif

#ifdef _ADJUST_SIZES
static unsigned int ADJUST_SIZE(unsigned int size)
{
    unsigned int SIZE = 256;
    unsigned int CNT  = (size / SIZE) + ((size % SIZE)?1:0);
    return (CNT * SIZE);
}
static unsigned int ADJUST_BLOCKS(unsigned int blocks, unsigned int rsize)
{
    const unsigned int MIN_NR_BLOCKS = 256;
    unsigned int nr_blocks = (blocks * NR_PAGE_SIZE) / rsize;
    if(nr_blocks < MIN_NR_BLOCKS) {
        // This is from algebra, substitute 256 for nr_blocks and solve for blocks in the above.
        return (rsize * MIN_NR_BLOCKS)/NR_PAGE_SIZE;
    } else {
        return blocks;
    }
}
#else
static unsigned int ADJUST_SIZE(unsigned int size)
{
    return size;
}
static unsigned int ADJUST_BLOCKS(unsigned int blocks, unsigned int rsize)
{
    return blocks;
}
#endif

/*
 * malloc
 *
 * Allocate memory using underlying block 
 * allocator.
 *
 */

#define NR_BLOCKS(SIZE) (((SIZE) / NR_PAGE_SIZE) + (((SIZE) % NR_PAGE_SIZE)?1:0))
 
void *balloc(unsigned int size)
{
    register int i = 0, j = 0, k = 0;
    register header_t *hdr = 0;
    // size is what the user wants, we increase it by the header and then make sure its on a pointer length boundary.
    unsigned int psize  = sizeof(char *);
    unsigned int tmp    = (ADJUST_SIZE(size) + sizeof(header_t));
    unsigned int rsize  = (((tmp / psize) + ((tmp % psize)?1:0))) * psize; // Align on pointer boundary.
    unsigned int blocks = ADJUST_BLOCKS((rsize / NR_PAGE_SIZE) + ((rsize % NR_PAGE_SIZE)?1:0),rsize);
#ifdef _USE_THREAD_STATIC_GCC
    static __thread unsigned int nr_slot= 0;
    static __thread mem_t *ptr = NULL;
#elif _USE_THREAD_STATIC_VS
    __declspec(thread) unsigned int nr_slot= 0;
    __declspec(thread) mem_t *ptr = NULL;
#else
    static   unsigned int nr_slot= 0;
    static   mem_t *ptr = NULL;
#endif
    register table_t *tptr = NULL;
    register char *mptr = NULL;

#ifdef _DEBUG_MALLOC
    printk("psize=%d\ntmp=%d\nrsize=%d\nblocks=%d\n",
            psize,tmp,rsize,blocks);
#endif

    asm_disable_interrupt();
    // - Try to allocate from slot we last allocated.
    if(ptr) {
        if(size <= (ptr->rsize - sizeof(header_t)) &&  // We can accommodate the request.
           ptr->nr_blocks > ptr->nr_alloc) { // This pool has at least one block available.
           // Allocate.
           for(k = 0, mptr = (char *)ptr->memory; k < ptr->nr_blocks; ++k, mptr += ptr->rsize) {
               hdr = (header_t *)mptr;
               if(hdr->is_free == FREE_MEM) {
                    PLINE(); // Case we have a slot that can fill the request.
                    hdr->is_free = USED_MEM;
                    hdr->slot = nr_slot; // Set from previous run.
                    ptr->nr_alloc++;
#ifdef _DEBUG_MALLOC
                    printk("hdr->is_free=%d\nhdr->slot=%d\nptr->nr_alloc=%d\n",
                            hdr->is_free,hdr->slot,ptr->nr_alloc);
#endif
                    asm_enable_interrupt();
                    return ((char *)hdr + sizeof(header_t));
               }
           }
        }
    }
    if(!root) {
        PLINE(); // Case init.
        root = (table_t *)block(NR_BLOCKS(sizeof(table_t)));
        root->next = NULL;
        root->nr_slots = 1;
        for(i = 0; i < NR_SIZES; ++i) {
            root->sizes[i] = zero_mem;
        }
        ptr = &root->sizes[0]; // Set for next run.
        ptr->size   = size;  // size the user wants which can accomodate any size less than or equal to
        ptr->rsize  = rsize; // actual block size
        ptr->memory = block(blocks); // allocate the memory
        ptr->nr_pages  = blocks;
        ptr->nr_blocks = (blocks * NR_PAGE_SIZE) / rsize; // number of available blocks
        ptr->nr_alloc = 1; // number of allocated blocks
        // initialize the blocks
        for(i = 0, mptr = (char *)ptr->memory; i < ptr->nr_blocks; ++i, mptr += rsize) {
            hdr = (header_t *)mptr;
            hdr->slot = 0;
            hdr->is_free = FREE_MEM;
        }
        hdr = (header_t *)ptr->memory;
        hdr->is_free = USED_MEM;
        nr_slot = 0; // Set for next run.
        asm_enable_interrupt();
        return ((char *)hdr + sizeof(header_t));
    }
    // Scan the table, if we find a mem_t that can fill our request, allocate,
    // otherwise, create a new mem_t to full-fill the request.
    for(tptr = root, j = 0; tptr; tptr = tptr->next, ++j) {
        nr_slot = (NR_SIZES * j);
        for(i = 0; i < NR_SIZES; ++i) {
            ptr = &tptr->sizes[i]; // Set for next run.
            if(size <= (ptr->rsize - sizeof(header_t)) &&  // We can accommodate the request.
               ptr->nr_blocks > ptr->nr_alloc) { // This pool has at least one block available.
                // Allocate.
                for(k = 0, mptr = (char *)ptr->memory; k < ptr->nr_blocks; ++k, mptr += ptr->rsize) {
                    hdr = (header_t *)mptr;
                    if(hdr->is_free == FREE_MEM) {
                        PLINE(); // Case we have a slot that can fill the request.
                        hdr->is_free = USED_MEM;
                        hdr->slot = nr_slot + i;
                        ptr->nr_alloc++;
#ifdef _DEBUG_MALLOC
                        printk("hdr->is_free=%d\nhdr->slot=%d\nptr->nr_alloc=%d\n",
                                hdr->is_free,hdr->slot,ptr->nr_alloc);
#endif
			            nr_slot += i; // Set for next run.
                        asm_enable_interrupt();
                        return ((char *)hdr + sizeof(header_t));
                    }
                }
            }
        }
    }

    // Nothing in the pool to allocate from; we need a new block to handle the request.
    for(tptr = root, j = 0; tptr; tptr = tptr->next, ++j) {
        nr_slot = (NR_SIZES * j);
        for(i = 0; i < NR_SIZES && tptr->nr_slots != NR_SIZES; ++i) {
            ptr = &tptr->sizes[i]; // Set for next run.
            if(ptr->nr_blocks == 0) {
                PLINE(); // Case we need a new slot to allocate a new request as non match in our prior allocations.
                // Free slot; allocate here.
                tptr->nr_slots++;
                ptr->size = size;
                ptr->rsize= rsize;
                ptr->memory=block(blocks);
                ptr->nr_pages  = blocks;
                ptr->nr_blocks = (blocks * NR_PAGE_SIZE) / rsize;
                ptr->nr_alloc = 1;
                // initialize the blocks
                for(k = 0, mptr = (char *)ptr->memory; k < ptr->nr_blocks; ++k, mptr += ptr->rsize) {
                    hdr = (header_t *)mptr;
                    hdr->slot = 0; // This is assigned in the scan. See above.
                    hdr->is_free = FREE_MEM;
                }
                hdr = (header_t *)ptr->memory;
                hdr->is_free = USED_MEM;
                hdr->slot = nr_slot + i;
		        nr_slot += i; // Set for next run.
                asm_enable_interrupt();
                return ((char *)hdr + sizeof(header_t));
            }
        }
    }

    // Allocate a new table as we are full.
    PLINE(); // Case we need a new table to fill the request.
    tptr = (table_t *)block(NR_BLOCKS(sizeof(table_t)));
    tptr->next = root;
    root = tptr; // We are inserting at the root so slots are in decreasing order, this has implications to how we free.
    root->nr_slots = 1;
    for(i = 0; i < NR_SIZES; ++i) {
        root->sizes[i] = zero_mem;
    }
    ptr = &root->sizes[0];
    ptr->size   = size;          // size the user wants which can accomodate any size less than or equal to
    ptr->rsize  = rsize;         // actual block size
    ptr->memory = block(blocks); // allocate the memory
    ptr->nr_pages  = blocks;
    ptr->nr_blocks = (blocks * NR_PAGE_SIZE) / rsize; // number of available blocks
    ptr->nr_alloc = 1; // number of allocated blocks
    // initialize the blocks
    for(i = 0, mptr = (char *)ptr->memory; i < ptr->nr_blocks; ++i, mptr += rsize) {
        hdr = (header_t *)mptr;
        hdr->slot = 0; // This is assigned in the scan. See above.
        hdr->is_free = FREE_MEM;
    }
    hdr = (header_t *)ptr->memory;
    hdr->is_free = USED_MEM;
    hdr->slot = (NR_SIZES * j); // j should have the correct value from the prior scan.
#ifdef _DEBUG_MALLOC
    printk("ptr->size=%d\nptr->rsize=%d\nptr->nr_blocks=%d\nptr->nr_alloc=%d\nhdr->slot=%d\n",
            ptr->size,ptr->rsize,ptr->nr_blocks,ptr->nr_alloc,hdr->slot);
#endif
    nr_slot = (NR_SIZES * j); // Set for next run.
    asm_enable_interrupt();
    return ((char *)hdr + sizeof(header_t));
}

void bfree(void *addr)
{
    // Free using the addr slot, if an entire memory is free, release it using block_free.
    register header_t *hdr = (header_t *)((char *)addr - sizeof(header_t));
    unsigned int nr_table = hdr->slot / NR_SIZES, nr_slot = hdr->slot % NR_SIZES;
    unsigned int j = nr_table;
    register table_t *tptr = NULL;
    register mem_t *ptr = NULL;

#ifdef _DEBUG_MALLOC
    printk("nr_table=%d\nnr_slot=%d\n",nr_table,nr_slot);

    asm_disable_interrupt();
    if(hdr->is_free != USED_MEM) {
        printk("error freeing memory block invalid\n");
        asm_enable_interrupt();
        return;
    }
#endif

    hdr->is_free = FREE_MEM;
    for(tptr = root; tptr; tptr = tptr->next, --j) {
        if(nr_table == (NR_SIZES * j)) {
            PLINE();
            ptr = &tptr->sizes[nr_slot];
            tptr->nr_slots--;
            ptr->nr_alloc--;
#ifdef _DEBUG_MALLOC
            printk("ptr->nr_blocks=%d\nptr->nr_alloc=%d\nptr->size=%d\nptr->rsize=%d\n",
                    ptr->nr_blocks,ptr->nr_alloc,ptr->size,ptr->rsize);
#endif
            if(ptr->nr_alloc == 0) {
                PLINE();
                block_free(ptr->memory, ptr->nr_pages);
                ptr->memory = 0;
                ptr->nr_blocks = 0;
                ptr->size  = 0;
                ptr->rsize = 0;
            }
        }
    }
    asm_enable_interrupt();
}

void *malloc(unsigned int size)
{
    return balloc(size);
}

void free(void *addr)
{
    bfree(addr);
}

void malloc_unit_test()
{
    int i = 0;
    char *ptr, *ptr1, *ptr2, *ptr3, buf[128]={0};
    char **table = 0;

    printk("expect line 92\n");
    {
        ptr = (char *)balloc(10);
        strcpy(ptr,"hello");
        printk("%s\n", ptr);
        printk("expect line 125\n");
        {
            ptr1 = (char *)balloc(10);
            strcpy(ptr1,"world");
            printk("%s\n", ptr1);
        }
        printk("expect line 146\n");
        {
            ptr2 = (char *)balloc(20);
            strcpy(ptr2,"hello world");
            printk("%s\n",ptr2);
        }
        // TODO -
        // Allocate as many requests as there are slots to force
        // the next case for testing. This should also shed light
        // on how to optimize, for example, our size should probably be
        // larger than the user request and the number of blocks allocated
        // should probably be larger (or at least these things should be adjustable).
        // - This appears to work.
        // - Also test the case were we have a block larger than page size...
        printk("expect 170\n");
        {
            ptr3 = balloc(300);
            strcpy(ptr3, "this is another test of the emergency broadcast system\n");
            printk(ptr3);
        }
    }
    printk("expect 211\n");
    // *(ptr-4) = 0; // This tests if we are feeing an invalid block...
    bfree(ptr);
    printk("expect 211\n");
    bfree(ptr1);
    printk("expect 211\n");
    bfree(ptr2);
    printk("expect 211\n");
    bfree(ptr3);

    // - Allocate an array of C strings, write a count in them.
    //   Check the value of the strings.
    //   Free everything.
    //   Do it for several thousand iterations and on strings
    //   of increasing length, say 2,4,6, up to 2^14.
    table = (char **)balloc(1024 * sizeof(char **));
    for(i = 0; i < 1024; ++i) {
        table[i] = (char *)balloc(2 * (i+1) * 100);
        sprintk(table[i],"%d",i);
    }     
    for(i = 0; i < 1024; ++i) {
        sprintk(buf,"%d",i);
        if(strcmp(table[i],buf)) {
            printk("allocation error slot [%d]\n",i);
            break;
        }
    }
    if(i == 1024) {
        printk("allocation success\n");
    }
    for(i = 0; i < 1024; ++i) {
        bfree(table[i]);
    }
    bfree(table);
}
