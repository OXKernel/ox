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
//      inode.c
//
// @description:
//      Implementation of inodes. This is an intermediary
//      step for implementing the file system, above the
//      block and dev functionality and utilizing the path
//      code.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
// TODO: - dev calls will not work as the interface has changed.
//         DONE change dev_write to include the block 
//       - change the use of 4096 to DEV_BLOCK_SIZE (512 bytes) based on the macro.
//         DONE
//       - how to organize the free block list and inode list as disks
//         are rather large these days and we never could place the whole
//         thing in RAM anyways... The bitmaps must be on disk.
//         We would have to have algorithms to scan for free inode and
//         free disk block using the bitmaps. So something like :=
//
//         master | inode map | block map | inode blocks | data blocks
//
//         Create the file system given the size of the disk and
//         a calculation on how many inodes to create, the rest are
//         data blocks.
//
//       - replace the malloc calls with scans of the region for
//         a find of a free block we can not place the bitmaps in memory.
//
//       - find where the master boot record is (I think its block 0 so
//         the master inode starts at block 1)
//
//       - write the create fs such that we have a run of say 1 meg on
//         disk that contains the kernel before the file system starts
//         and that this is much easier to boot load from our boot loader
//
// TODO:
//       - Check all return variables of the block functions.
//       - Implement imap and bmap read.
//         DONE
//       - Implement bitmanipulation routines to set/get a bit in the maps.
//         DONE
//       - Implement the remaining functions.
//       - Test dev, block, and inode functionality.
//       - With this, we can implement the higher level 
//         fs calls for open,close,read,write,sync, etc.
//
#include "bool.h"
#include "paths.h"
#include "block.h"
#include "dev.h"
#include "inode.h"
#include "compat.h"

#ifdef _TEST_INODE_INC
#include <stdio.h>
#include <string.h>
#define printk printf
#else
#include <ox/error_rpt.h>
#include <errno.h>
#endif

static master_inode_t master_tab[INODE_NR_DEV];
static char master_bmap_memory[INODE_NR_DEV][DEV_BLOCK_SIZE];
static char master_imap_memory[INODE_NR_DEV][DEV_BLOCK_SIZE];
static bool init_master = false;
static struct master_dev {
        char *path;
        int    dev;
} master_dev_tab[MAX_DEV]={0};

void master_init()
{
    register int i = 0;
    char zero_mem[DEV_BLOCK_SIZE] = {0};
    if(!init_master) {
        printk("init master...\n"
               "sizeof master=%d block size=%d\n",sizeof(master_inode_t),DEV_BLOCK_SIZE);
        for(; i < INODE_NR_DEV; ++i) {
            memset(&master_tab[i],0x0,DEV_BLOCK_SIZE);
            memset(master_bmap_memory[i],0x0,DEV_BLOCK_SIZE);
            memset(master_imap_memory[i],0x0,DEV_BLOCK_SIZE);
            master_tab[i].dev = INODE_NOPOS;
        }
        init_master = true;
    }
}

master_inode_t *master_get(int dev)
{
    register int i = 0;
    for(; i < INODE_NR_DEV; ++i) {
        if(master_tab[i].dev == dev) {
            return &master_tab[i];
        }
    }
    return NULL;
}

//
// master_get_mem_ptr:
// Return an index into the master_bmap_memory 
// and master_imap_memory memory structures
// so as to allocate.
int master_get_mem_ptr(int dev)
{
    register int i = 0;
    for(; i < INODE_NR_DEV; ++i) {
        if(master_tab[i].dev == dev) {
            return i;
        }
    }
    // Not found.
    return INODE_NOPOS;
}

master_inode_t *master_alloc(int dev)
{
    register int i = 0;
    master_inode_t *m = master_get(dev);
    if(m) {
        return m;
    } else {
        for(; i < INODE_NR_DEV; ++i) {
            if(master_tab[i].dev == INODE_NOPOS) {
                master_tab[i].dev = dev;
                return &master_tab[i];
            }
        }
    }
    // Could not allocate.
    return NULL;
}

void master_free(int dev)
{
    master_inode_t *m = master_get(dev);
    if(m) {
        memset(m, 0x0, DEV_BLOCK_SIZE);
        m->dev = INODE_NOPOS;
    }
}

int master_set_dev(char *path, int dev)
{
    int i = 0;
    for(; i < MAX_DEV; ++i) {
        if(!master_dev_tab[i].path || !strlen(master_dev_tab[i].path)) {
            master_dev_tab[i].path = path;
            master_dev_tab[i].dev  = dev;
            return i;
        }
    }
    return -1;
}

// TODO - It might be that we resolve the full path and then try to
// find a device for each component of the path, right now we just
// have one path to one device mapping.
int master_get_dev(char *path)
{
    int i = 0;
    for(; i < MAX_DEV; ++i) {
#if 0
        if(path && !strcmp(master_dev_tab[i].path, path)) {
            return master_dev_tab[i].dev;
        }
#endif
        // TODO - Just one device for now.
        if(strlen(master_dev_tab[i].path)) {
            return master_dev_tab[i].dev;
        }
    }
    if(!path) {
        // Null path implies root.
        for(; i < MAX_DEV; ++i) {
            // TODO - Just one device for now.
            if(strlen(master_dev_tab[i].path)) {
                return master_dev_tab[i].dev;
            }
        }
    }
    return -1;
}

//
// inode_create_fs:
// 
// This is logic that creates a file system.
// However, the first sector (sector 0) is the MBR
// followed by the boot loader and kernel.
// Thus we start at block_start not block 0.
// The term block here refers to a sector, but is
// logically numbered from 0 to n-1 with the expectation
// that the underlying disk driver will convert this
// numbering scheme into the appropriate sector/cylinder/head
// numbering scheme. This assumption is also made in the
// block.c (buffer cache code).
//
// path := filename of the fs (in user space, this is a regular filename)
//         in kernel, this can be used to open a specific device
//
// block_start := the actual block we start at, in user space, it can
//                be 0, in kernel space, we reserve the first sectors
//                for the MBR, boot, kernel images.
//                Currently, we reserve 2 MB (which is the size of
//                a 1.44 MB floppy drive unformatted, but also gives
//                us a nice (2*2^20)/512 = 4096 block as a start).
//
// size := the size from block_start to end of diskspace
//         this is the amount of space that the file system actually
//         has. Total is (block_start * DEV_BLOCK_START) + size.
//         Currently, this will be disk size - 2 MB.
//
inode_rtvl_t inode_mkfs(char *path, block_t size)
{
    // TODO - This information should also be in our boot loader.
    return inode_create_fs(path, BLOCK_START, size - TWOMEG);
}

inode_rtvl_t inode_create_fs(char *path, block_t block_start, block_t size)
{
   block_t i      = 0;
   block_t dev    = 0;
   block_t inodes = 0;
   block_t blocks = 0;
   block_t imap   = 0;
   block_t bmap   = 0;

   char zero_data[DEV_BLOCK_SIZE] = {0};

   master_inode_t *master = NULL;
   /* Initialize master structures. */
   master_init();
   /* Assert that the size is divisible by the page size. */
   if((size % DEV_BLOCK_SIZE) != 0) {
      printk("inode_create_fs:: size=%d not aligned on page size=%d\n",size,DEV_BLOCK_SIZE);
      return INODE_FAIL;
   }
   dev_open(path, &dev);
   /* Create the header (block map, inode map, inodes).   */
   /* Determine how many blocks are needed for the 
    * maps and inodes. We have a formula of 20% inode
    * 80% data blocks.
    */
   size /= DEV_BLOCK_SIZE; /* Assume size is bytes, calculate number of blocks. */
   inodes = size * 20 / 100; /* Amount of space needed for inodes. */
   // We want integer division to round down, removing 'inodes++;'
   imap = inodes / 8; /* Number of chars needed for map. */
   imap /= DEV_BLOCK_SIZE; /* Number of blocks needed for map. */
   if(!imap) ++imap;
   // We want integer division to round down, removing 'imap++;'
   blocks = size * 80 / 100; /* Amount of space needed for blocks. */
   bmap = blocks / 8; /* Number of chars needed for map. */
   bmap /= DEV_BLOCK_SIZE; /* Number of blocks needed for map. */
   if(!bmap) ++bmap;
   // We want integer division to round down, removing 'bmap++;'
   /* Initialize the master inode. */
   master_alloc(dev);
   master = master_get(dev);
   master->init_fs = 1;
   master->magic = INODE_MAGIC;
   master->block_start = block_start;
   master->inode_map_start = block_start + 1; // Skip master inode.
   master->inode_map_end = master->inode_map_start + imap;
   master->bmap_map_start = master->inode_map_end;
   master->bmap_map_end = master->bmap_map_start + bmap;
   master->inode_start = master->bmap_map_end;
   master->inode_end = master->inode_start + inodes;
   master->data_start= master->inode_end;
   master->data_end = master->data_start + blocks;
   master->imap_ptr = 0;
   master->bmap_ptr = 0;
   master->imap_bit = 0;
   master->bmap_bit = 0;
   master->imap = 0;
   master->bmap = 0;
   master->inodes = inodes;
   master->blocks = blocks;
   memset(master->pad, 0x0, MNODE_PAD);
   /* Write it to disk.  */
   printk("inode_mkfs:: block_start=%d\n",block_start);
   dev_scan(dev, block_start);
   dev_write(dev, block_start, (char *)master);
   //master->magic = 0; // RGDDEBUG
   //dev_read(dev, block_start, (char *)master); // RGDDEBUG
   //if(master->magic != INODE_MAGIC) { // RGDDEBUG
   //     panic("error writing master...\n"); // RGDDEBUG
   //} // RGDDEBUG
   //panic("halt..."); // RGDDEBUG
   /* Initialize the maps and inodes to be 0. */
   printk("inode_mkfs:: size=%u\n"
          "             master->inode_map_start=%u\n"
          "             master->inode_map_end=%u\n"
          "             master->bmap_map_start=%u\n"
          "             master->bmap_map_end=%u\n"
          "             master->inode_start=%u\n"
          "             master->inode_end=%u\n"
          "             master->inodes=%u\n"
          "             master->blocks=%u\n",
          size,
          master->inode_map_start,
          master->inode_map_end,
          master->bmap_map_start,
          master->bmap_map_end,
          master->inode_start,
          master->inode_end,
          master->inodes,
          master->blocks);
   for(i = master->inode_map_start; i < master->inode_map_end; i++) {
      dev_scan(dev,i);
      dev_write(dev,i,zero_data);
   }
   for(i = master->bmap_map_start; i < master->bmap_map_end; i++) {
      dev_scan(dev,i);
      dev_write(dev,i,zero_data);
   }
   printk("inode_start=%d inode_end=%d\n",
        master->inode_start,
        master->inode_end);
#if 0
   for(i = master->inode_start; i < master->inode_end; i++) {
      dev_scan(dev,i);
      dev_write(dev,i,zero_data);
   }
#endif
   block_reinit();
   dev_scan(dev,0);
   dev_close(dev);
   master_free(dev);
   return INODE_OK;
}

inode_rtvl_t inode_dev_open(char *path, int *dev)
{
   int i    = 0;
   block_t size = 0;
   master_inode_t *master = NULL;

   /* Open the device and load the master inode. */
   if(block_open(path,dev) != BLOCK_OK) {
      printk("inode_dev_open:: block_open failed path=%s\n", path);
      return INODE_FAIL;
   }
   // Allocate a master block.
   printk("inode_dev_open:: dev=%d\n",*dev);
   master_init();
   master_alloc(*dev);
   master = master_get(*dev);
   if(master == NULL) {
        dev_close(*dev);
        printk("inode_dev_open:: failed to allocate master\n");
        return INODE_FAIL;
   }
   // First try our BLOCK_START.
   if(block_read(*dev,BLOCK_START,(char *)master) != BLOCK_OK) {
      errno = EACCES;
      dev_close(*dev);
      printk("inode_dev_open:: failed to read master BLOCK_START=%d\n",
              BLOCK_START);
      return INODE_FAIL;
   }
   printk("inode_dev_open:: master->magic=%d INODE_MAGIC=%d\n",master->magic, INODE_MAGIC);
   if(master->magic != INODE_MAGIC) {
        // Magic number wasn't there, try without the cache.
        if(dev_read(*dev,BLOCK_START,(char *)master) != DEV_OK) {
            errno = EACCES;
            dev_close(*dev);
            printk("inode_dev_open:: failed to read master BLOCK_START=4096\n");
            return INODE_INIT;
        }
        // Failed, this device doesn't have this file system on it.
        if(master->magic != INODE_MAGIC) {
            master_free(*dev);
            dev_close(*dev);
            printk("inode_dev_open:: failed master->magic=%d expected INODE_MAGIC=%d\n",master->magic, INODE_MAGIC);
            return INODE_INIT;
        }
   }
   if(master->init_fs != 1) {
      master_free(*dev);
      dev_close(*dev);
      printk("inode_dev_open:: filesystem not initialized master->init_fs=%d\n",
             master->init_fs);
      return INODE_INIT;
   }
   /* Setup the imap and bmap for runtime use. */
   /* We can not load the entire maps in ram,
    * load from the pointers and use a single block of 512 bytes.
    */
   i = master_get_mem_ptr(*dev);
   if(i == INODE_NOPOS) {
        master_free(*dev);
        dev_close(*dev);
        printk("inode_dev_open:: failed to get memory=%d\n",i);
        return INODE_FAIL;
   }
   master->bmap = master_bmap_memory[i];
   if(master->bmap_ptr == 0) {
        master->bmap_ptr = master->bmap_map_start;
        master->bmap_bit = 0;
   }
   if(block_read(*dev, master->bmap_ptr, (char *)master->bmap) != BLOCK_OK) {
        errno = EACCES;
        master_free(*dev);
        dev_close(*dev);
        printk("inode_dev_open:: block_read failed dev=%d block=%u\n",
                *dev, master->bmap_ptr);
        return INODE_FAIL;
   }
   master->imap = master_imap_memory[i];
   if(master->imap_ptr == 0) {
        master->imap_ptr = master->inode_map_start;
        master->imap_bit = 0;
   }
   if(block_read(*dev, master->imap_ptr, (char *)master->imap) != BLOCK_OK) {
        errno = EACCES;
        master_free(*dev);
        dev_close(*dev);
        printk("inode_dev_open:: block_read failed dev=%d block=%u\n",
                *dev, master->imap_ptr);
        return INODE_FAIL;
   }
   return INODE_OK;
}

inode_rtvl_t inode_get_data_block(int dev, block_t *block)
{
   register master_inode_t *master = master_get(dev);
   register block_t ptr = 0;
   register block_t bit = 0;
   register block_t pass= 0;
   register int i = 0;

   if(!master) {
        printk("inode_get_data_block:: failed to get master dev=%d\n", dev);
        return INODE_FAIL;
   }
   ptr = master->bmap_ptr;
   bit = master->bmap_bit;
   *block = INODE_NOPOS;
   if(master->bmap_ptr == 0) {
       // Initialize.
       if(!master->init_fs) {
            printk("inode_get_data_block:: invalid device\n");
            return INODE_FAIL;
       }
       master->bmap_ptr = master->bmap_map_start;
       i = master_get_mem_ptr(dev);
       if(i == INODE_NOPOS) {
            printk("inode_get_data_block:: failed to get memory\n");
            return INODE_FAIL;
       }
       master->bmap = master_bmap_memory[i];
       if(block_read(dev, master->bmap_ptr, (char *)master->bmap) != BLOCK_OK) {
            errno = EACCES;
            P();
            printk("inode_get_data_block:: block_read failed dev=%d block=%u\n",
                    dev, master->bmap_ptr);
            return INODE_FAIL;
       }
       ptr = master->bmap_ptr;
       bit = 0;
   }

   do {
       if(bit_get(master->bmap, bit) == 0) {
           bit_set(master->bmap, bit, 1);
           master->bmap_bit = bit;
           if(block_write(dev, master->block_start, (char *)master) != BLOCK_OK) {
                P();
                errno = EACCES;
                printk("inode_get_data_block:: block_write failed dev=%d block=%u\n",
                   dev, master->block_start);
                return INODE_FAIL;
           }
           if(block_write(dev, ptr, (char *)master->bmap) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_get_data_block:: block_write failed dev=%d block=%u\n",
                   dev, master->block_start);
                return INODE_FAIL;
           }
           // block_calculation
           *block = master->data_start +
                   ((ptr - master->bmap_map_start) * DEV_BLOCK_SIZE) +
                    bit; 
            return INODE_OK;
       }
       ++bit;
       bit %= DEV_BLOCK_SIZE;
       master->bmap_bit = bit;
       if(bit == 0) {
           ptr++;
           if(ptr < master->bmap_map_end) {
               master->bmap_ptr = ptr;
               if(block_read(dev, master->bmap_ptr, 
                                  (char *)master->bmap) != BLOCK_OK) {
                  P();
                  errno = EACCES;
                  printk("inode_get_data_block:: block_read failed dev=%d block=%u\n",
                        dev, master->bmap_ptr);
                    return INODE_FAIL;
               }
           } else {
               ptr = master->bmap_ptr = master->bmap_map_start;
               if(block_read(dev, master->bmap_ptr, 
                                  (char *)master->bmap) != BLOCK_OK) {
                  P();
                  errno = EACCES;
                  printk("inode_get_data_block:: block_read failed dev=%d block=%u\n",
                        dev, master->bmap_ptr);
                    return INODE_FAIL;
               }
               pass++;
           }
       }
   } while(pass < INODE_NR_PASS);
   *block = INODE_NULL;
   return INODE_FAIL;
}

inode_rtvl_t inode_get_inode_block(int dev, block_t *block)
{
   register master_inode_t *master = master_get(dev);
   register block_t ptr = 0;
   register block_t bit = 0;
   register block_t pass= 0;
   register int i = 0;

   if(!master) {
       printk("inode_get_inode_block:: failed to get master dev=%d\n",dev);
       return INODE_FAIL;
   }
   ptr = master->imap_ptr;
   bit = master->imap_bit;
   *block = INODE_NOPOS;

   if(master->imap_ptr == 0) {
       // Initialize.
       if(!master->init_fs) {
            printk("inode_get_data_block:: invalid device\n");
            return INODE_FAIL;
       }
       master->imap_ptr = master->inode_map_start;
       i = master_get_mem_ptr(dev); 
       if(i == INODE_NOPOS) {
            printk("inode_get_inode_block:: failed to get memory\n");
            return INODE_FAIL;
       }
       master->imap = master_imap_memory[i];
       if(block_read(dev, master->imap_ptr, (char *)master->imap) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_get_inode_block:: block_read failed dev=%d block=%u\n",
                    dev, master->imap_ptr);
            return INODE_FAIL;
       }
       ptr = master->imap_ptr;
       bit = 0;
   }

   do {
       if(bit_get(master->imap, bit) == 0) {
           bit_set(master->imap, bit, 1);
           master->imap_bit = bit;
           if(block_write(dev, master->block_start, (char *)master) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_get_inode_block:: block_write failed dev=%d block=%u\n",
                   dev, master->block_start);
                return INODE_FAIL;
           }
           if(block_write(dev, ptr, (char *)master->imap) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_get_inode_block:: block_write failed dev=%d block=%u\n",
                   dev, master->imap);
                return INODE_FAIL;
           }
           // inode_block_calculation
           *block = master->inode_start + 
                   ((ptr - master->inode_map_start) * DEV_BLOCK_SIZE) +
                    bit; 
            return INODE_OK;
       }
       ++bit;
       bit %= DEV_BLOCK_SIZE;
       master->imap_bit = bit;
       if(bit == 0) {
           ptr++;
           if(ptr < master->inode_map_end) {
               master->imap_ptr = ptr;
               if(block_read(dev, master->imap_ptr, 
                                  (char *)master->imap) != BLOCK_OK) {
                    errno = EACCES;
                    printk("inode_get_inode_block:: block_read failed dev=%d block=%u\n",
                        dev, master->imap_ptr);
                    return INODE_FAIL;
               }
           } else {
               ptr = master->imap_ptr = master->inode_map_start;
               if(block_read(dev, master->imap_ptr, 
                                  (char *)master->imap) != BLOCK_OK) {
                    errno = EACCES;
                    printk("inode_get_inode_block:: block_read failed dev=%d block=%u\n",
                        dev, master->imap_ptr);
                    return INODE_FAIL;
               }
               pass++;
           }
       }
   } while(pass < INODE_NR_PASS);
   *block = INODE_NULL;
   return INODE_FAIL;
}

inode_rtvl_t inode_free_data_block(int dev, block_t block)
{
    master_inode_t *master = master_get(dev);
    register block_t ptr = 0;
    register block_t bit = 0;
    register int i = 0;

    if(!master) {
        printk("inode_free_data_block:: failed to get master dev=%d\n", dev);
        return INODE_FAIL;
    }
    ptr = master->bmap_ptr;
    bit = master->bmap_bit;
    if(master->bmap_ptr == 0) {
       // Initialize.
       master->bmap_ptr = master->bmap_map_start;
       i = master_get_mem_ptr(dev);
       if(i == INODE_NOPOS) {
            printk("inode_free_data_block:: failed to get memory\n");
            return INODE_FAIL;
       }
       master->bmap = master_bmap_memory[i];
       if(block_read(dev, master->bmap_ptr, (char *)master->bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_free_data_block:: block_read failed dev=%d block=%u\n",
                    dev, master->bmap_ptr);
            return INODE_FAIL;
       }
       ptr = master->bmap_ptr;
       bit = 0;
   }

   // Find ptr and bit given block number.
   // This is from math, see 'block_calculation' above.
   block -= master->data_start;
   ptr = block / DEV_BLOCK_SIZE + master->bmap_map_start;
   bit = block % DEV_BLOCK_SIZE;
   if(ptr != master->bmap_ptr) {
        // We are not referencing it, load it.
        master->bmap_ptr = ptr;
        master->bmap_bit = bit;
        if(block_read(dev, master->bmap_ptr, (char *)master->bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_free_data_block:: block_read failed dev=%d block=%u\n",
                    dev, master->bmap_ptr);
            return INODE_FAIL;
        }
   } else if(ptr == master->bmap_ptr && bit != master->bmap_bit) {
        // Setup the bit.
        master->bmap_bit = bit;
   }
   if(bit_get(master->bmap, bit) == 0) {
       printk("inode_free_data_block:: warning free'ing free block=%u bit=%d\n", ptr, bit);
       return INODE_OK;
   }
   // All we have to do is set this to zero and the block
   // is free'd. This is how we unlink files, we just set the bit
   // to free for the inode.
   bit_set(master->bmap, bit, 0);
   return INODE_OK;
}

inode_rtvl_t inode_free_inode_block(int dev, block_t block)
{
    master_inode_t *master = master_get(dev);
    register block_t ptr = 0;
    register block_t bit = 0;
    register int i = 0;

    if(!master) {
        printk("inode_free_inode_block:: failed to get master dev=%d\n", dev);
        return INODE_FAIL;
    }
    ptr = master->imap_ptr;
    bit = master->imap_bit;
    if(master->imap_ptr == 0) {
       // Initialize.
       master->imap_ptr = master->inode_map_start;
       i = master_get_mem_ptr(dev);
       if(i == INODE_NOPOS) {
            printk("inode_free_inode_block:: failed to get memory\n");
            return INODE_FAIL;
       }
       master->imap = master_imap_memory[i];
       if(block_read(dev, master->imap_ptr, (char *)master->imap) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_free_inode_block:: block_read failed dev=%d block=%u\n",
                    dev, master->imap_ptr);
            return INODE_FAIL;
       }
       ptr = master->imap_ptr;
       bit = 0;
   }

   // Find ptr and bit given block number.
   // This is from math, see 'inode_block_calculation' above.
   block -= master->inode_start;
   ptr = block / DEV_BLOCK_SIZE + master->inode_map_start;
   bit = block % DEV_BLOCK_SIZE;
   if(ptr != master->imap_ptr) {
        // We are not referencing it, load it.
        master->imap_ptr = ptr;
        master->imap_bit = bit;
        if(block_read(dev, master->imap_ptr, (char *)master->imap) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_free_inode_block:: block_read failed dev=%d block=%u\n",
                    dev, master->imap_ptr);
            return INODE_FAIL;
        }
   } else if(ptr == master->imap_ptr && bit != master->imap_bit) {
        // Setup the bit.
        master->imap_bit = bit;
   }
   if(bit_get(master->imap, bit) == 0) {
       printk("inode_free_inode_block:: warning free'ing free inode block=%u bit=%d\n", ptr, bit);
       return INODE_OK;
   }
   // All we have to do is set this to zero and the block
   // is free'd. This is how we unlink files, we just set the bit
   // to free for the inode.
   bit_set(master->imap, bit, 0);
   return INODE_OK;
}

inode_rtvl_t inode_dev_close(int dev)
{
   int i = 0;
   master_inode_t *master = master_get(dev);
   if(!master) {
        printk("inode_dev_close:: failed to get master dev=%d\n", dev);
        return INODE_FAIL;
   }
   if(block_write(dev, master->imap_ptr, (char *)master->imap) != BLOCK_OK) {
        errno = EACCES;
        printk("inode_dev_close:: block_write failed dev=%d block=%u\n", dev, master->imap_ptr);
        return INODE_FAIL;
   }
   if(block_write(dev, master->bmap_ptr, (char *)master->bmap) != BLOCK_OK) {
        errno = EACCES;
        printk("inode_dev_close:: block_write failed dev=%d block=%u\n", dev, master->bmap_ptr);
        return INODE_FAIL;
   }
   // Write at block_start.
   if(block_write(dev, master->block_start, (char *)master) != BLOCK_OK) {
        errno = EACCES;
        printk("inode_dev_close:: block_write failed dev=%d block=%u\n", dev, master->block_start);
        return INODE_FAIL;
   }
   if(block_close(dev) != BLOCK_OK) {
        printk("inode_dev_close:: block_close failed\n");
        return INODE_FAIL;
   }

   for(i = 0; i < MAX_DEV; ++i) {
        if(master_dev_tab[i].dev == dev) {
            master_dev_tab[i].path = NULL;
            master_dev_tab[i].dev = 0;
        }
   }

   master_free(dev);
   return INODE_OK;
}

// TODO - Double check that this code works, it was not tested.
// inode_get:
//
// Given a path, obtain the corresponding inode. This is also known
// as namei in other filesystems.
//
// RGDTODO - This whole thing has to be rewritten. We want to use krealpath
//           to resolve the path
//           Then like inode_create construct a table of paths and walk them
//           Each one, use the scan_code to find the next link making sure
//           the components are a directory if not at the end of the path
//           and if its a link, process only symlink as we do not permit
//           hardlinks to directory. I think this pretty much adds support
//           for soft and hard links aside from adding the code to manage them in link.c
inode_rtvl_t inode_get(int dev, 
                       block_t current_dir, 
                       char *path, 
                       bool do_readlink, /* true => readlink, false => get the link itself */
                       inode_mode_t mode,
                       inode_t *res_inode)
{
       int  start = 0, tmp = 0, i = 0, j = 0, ptr_tab_len = 0;
       char res_path[MAX_PATH]={0}, in_path[MAX_PATH]={0}, *ptr_tab[MAX_PATH]={0}; 
       block_map_t bmap={0}, zero_bmap={0};
       block_t block = 0, parent = 0, current = 0, found = 0;
       inode_t inode ={0}, tnode = {0}, zero_node = {0};
       master_inode_t *master = master_get(dev);
       inode_perm_t perm = 0, umask = 0;

       if(!master) {
            printk("inode_get:: error getting master dev=%d\n");
            errno = EINVAL;
            return INODE_FAIL;
       }
       if(strlen(path) > MAX_PATH) {
            errno = ENAMETOOLONG;
            printk("inode_get:: error path name too long\n");
            return INODE_FAIL;
       }
       strcpy(in_path,path);
       if(!krealpath(dev, current_dir, path, res_path, &errno)) {
            printk("inode_get:: error resolving absolute path "
                   "dev=%d current_dir=%u path=%s res_path=%s error=%d\n",
                    dev, current_dir, path, res_path, errno);
            return INODE_FAIL;
       }
       // Parse the path getting every component and computing the
       // number of components.
       while((path=get_next_path(&start, res_path)) != NULL) {
            // RGDTODO - get_next_path may have to change to return "/" 
            // for the first path.
            if(path && !strlen(path) && ptr_tab_len == 0) {
                ptr_tab[ptr_tab_len++] = "/";
            } else {
                ptr_tab[ptr_tab_len++] = path;
            }
            start++;
       }
       // Check if we have root inode,
       if(block_read(dev, INODE_ROOT_BLOCK, (char *)&inode) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_get:: error reading block dev=%d root block=%u\n",
                    dev, INODE_ROOT_BLOCK);
            return INODE_FAIL;
       }
       if(strcmp(inode.path,"/")) {
          // Initialize.
          strcpy(inode.path,"/");
          inode.next = INODE_NULL;
          if(inode_set_permissions(&inode, 0777, 0000) != INODE_OK) {
                printk("inode_get:: error setting / permissions to 0777\n");
                errno = EPERM;
                return INODE_FAIL;
          }
          if(inode_set_timestamps(&inode) != INODE_OK) {
              printk("inode_get:: error setting timestamp\n");
              errno = EINVAL;
              return INODE_FAIL;
          }
          inode.is_directory = true;
          inode.dev = dev;
          inode.self = INODE_ROOT_BLOCK;
          inode.parent = INODE_ROOT_BLOCK;
          inode.group = 0;
          inode.owner = 0;
          if(inode_get_inode_block(dev, &block) != INODE_OK) {
                errno = EACCES;
                printk("inode_get:: error allocating inode block dev=%d block=%u\n",
                        dev, block);
                return INODE_FAIL;
          }
          if(block != INODE_ROOT_BLOCK) {
                printk("inode_get:: error allocating INODE_ROOT_BLOCK=%u block=%u\n",
                        INODE_ROOT_BLOCK,block);
                return INODE_FAIL;
          }
          if(block_write(dev, INODE_ROOT_BLOCK, (char *)&inode) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_get:: error creating root inode dev=%d\n", dev);
                return INODE_FAIL;
          }
       }
       // Rule out special case of getting just root.
       if(ptr_tab_len == 1 && !strcmp(ptr_tab[0],"/")) {
            // Struct assign.
            *res_inode = inode;
            return INODE_OK;
       }
       // At this point, we have the root inode loaded.
       // It serves as parent for the whole file system.
       // NOTE: We start at 1 at this point.
       for(i = 1, parent = inode.self; i < ptr_tab_len; ++i) {
            path = ptr_tab[i];
            // Look up the path component in the parent.
            if(block_read(dev, parent, (char *)&inode) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_get:: error reading block dev=%d parent=%u\n",
                        dev, parent);
                return INODE_FAIL;
            }
            // If parent is a directory and we are not at the end of the path.
            // RGDTODO - Check permissions as we consume the path.
            if((inode.is_directory || inode.is_symlink)) {
                if(inode.is_symlink) {
                    if((block=kreadlink(dev, inode.self, inode.path, &errno)) == INODE_NULL) {
                        printk("inode_get:: kreadlink error dev=%d inode.self=%u path=%s\n",
                                dev, inode.self, inode.path);
                        return INODE_FAIL;
                    }
                    if(block_read(dev, block, (char *)&inode) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_get:: error reading block dev=%d block=%u\n", dev, block);
                        return INODE_FAIL;
                    }
                    if(!inode.is_directory) {
                        printk("inode_get:: error symlink does not resolve to directory "
                               "dev=%d block=%u\n", dev, block);
                        errno = EACCES;
                        return INODE_FAIL;
                    }
                    parent = inode.self;
                }
                if(!inode_check_permissions(&inode, perm, umask, 
                                            current_process->group, 
                                            current_process->owner, INODE_RX)) {
                    errno = EACCES;
                    printk("inode_get:: INODE_RX permissions failed perm=%u umask=%u group=%u owner=%u\n",
                           perm, umask, current_process->group, current_process->owner);
                    return INODE_FAIL;
                }
                current = bmap.next = inode.next;
                if(inode.next == INODE_NULL) {
                    errno = ENOENT;
                    printk("inode_get:: file not found [%s]\n", in_path);
                    // Struct assign.
                    *res_inode = zero_node;
                    return INODE_FILE_NOT_FOUND;
                }
                block = INODE_NULL;
                found = INODE_NULL;
                do {
                    current = bmap.next;
                    if(block_read(dev, bmap.next, (char *)&bmap) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_get:: error reading block dev=%d block=%u\n",
                                dev, bmap.next);
                        return INODE_FAIL;
                    }
                    for(j = 0; j < BMAP_BLOCKS; j++) {
                        if(bmap.blocks[j] != INODE_NULL) {
                            if(block_read(dev, bmap.blocks[j], (char *)&tnode) != BLOCK_OK) {
                                errno = EACCES;
                                printk("inode_get:: error reading block dev=%d block=%u\n",
                                        dev, bmap.blocks[j]);
                                return INODE_FAIL;
                            }
                            if(!strcmp(tnode.path,path)) {
                                found = bmap.blocks[j];
                                break;
                            }
                        }
                    }
                } while(bmap.next != INODE_NULL && found == INODE_NULL);
                if(found != INODE_NULL) {
                    // We found it, we were not at the end of the path
                    // so go back in the loop using the found node
                    // as parent.
                    if(i == (ptr_tab_len-1)) {
		                // We are at the end of the path, check if we exist,
		                // We just return the inode if we are at the end,
		                // the directory lookup is needed only in 
                        // the beginning and middle.
		                if((tnode.is_symlink || tnode.is_hardlink) && do_readlink) {
                            printk("inode_get:: READING LINK block=%u path=%s\n",
                                    tnode.self, tnode.path);
		                    if((block=kreadlink(dev, tnode.self, tnode.path, &errno)) == INODE_NULL) {
		                        errno = ENOENT;
		                        printk("inode_get:: kreadlink error dev=%d current_dir=%u path=%s\n",
		                                dev, current_dir, tnode.path);
		                        return INODE_FAIL;
		                    }
		                    if(block_read(dev, block, (char *)&tnode) != BLOCK_OK) {
		                        errno = EACCES;
		                        printk("inode_get:: error reading block dev=%d block=%u\n", dev, block);
		                        return INODE_FAIL;
		                    }
		                }
		                if(!inode_check_permissions(&tnode, perm, umask, 
		                                            current_process->group, 
		                                            current_process->owner, mode)) {
		                   errno = EACCES;
		                   printk("inode_get:: perms failed mode=%d perm=%u umask=%u group=%u owner=%u\n",
		                           mode, perm, umask, current_process->group, current_process->owner);
		                   return INODE_FAIL;
		                }
                        *res_inode = tnode;
                        return INODE_OK;
                    } else {
                        parent = found;
                        continue;
                    }
                } else {
                    // We fail here as the path doesn't exist.
                    // Return no entry.
                    errno = ENOENT;
                    return INODE_FILE_NOT_FOUND;
                }
            } else if(!(inode.is_directory || inode.is_symlink) && (i != (ptr_tab_len-1))) {
                if(inode.is_file) {
                    // This is an error, the path contains a file.
                    errno = ENOENT;
                    printk("inode_get:: path contains a file [expected directory]\n");
                    return INODE_FAIL;
                } else {
                    // Can not have a hard link to a directory.
                    printk("inode_get:: path invalid\n");
                    errno = EINVAL;
                    return INODE_FAIL;
                }
            }
       }
       return INODE_FAIL;
}

inode_rtvl_t inode_set_parent_mod_time(int dev, block_t parent)
{
    inode_t inode={0};
    if(block_read(dev, parent, (char *)&inode) != BLOCK_OK) {
        printk("inode_set_parent_mod_time:: error reading block dev=%d block=%u\n",
                dev, parent);
        return INODE_FAIL;
    }
    inode.modified_time = ktime(0);
    return INODE_OK;
}

inode_rtvl_t inode_set_timestamps(inode_t *inode)
{
    // RGDTODOTIMESTAMPS
    // Actually implement this as ticks since the epoch
    // we need a driver in the pit controller to allow us
    // to get the ticks and another system call to get the time.
    inode->create_time = ktime(0);
    inode->modified_time = ktime(0);
    inode->accessed_time = ktime(0);
    // Add mod time to parent inode in inode_create and in inode_free.
    return INODE_OK;
}

// Convert our internal representation of permissions
// into unix permissions.
inode_perm_t inode_get_permissions(inode_t *inode)
{
    inode_perm_t perm = 0;
    if(inode->user_read) {
        perm |= S_IRUSR;
    }
    if(inode->user_write) {
        perm |= S_IWUSR;
    }
    if(inode->user_execute) {
        perm |= S_IXUSR;
    }
    // Group
    if(inode->group_read) {
        perm |= S_IRGRP;
    }
    if(inode->group_write) {
        perm |= S_IWGRP;
    }
    if(inode->group_execute) {
        perm |= S_IXGRP;
    }
    // World
    if(inode->world_read) {
        perm |= S_IROTH;
    }
    if(inode->world_write) {
        perm |= S_IWOTH;
    }
    if(inode->world_execute) {
        perm |= S_IXOTH;
    }
    // Is dir or link?
    if(inode->is_directory) {
        perm |= S_IFDIR;
    }
    if(inode->is_symlink) {
        perm |= S_IFLNK;
    }
    // Is regular file.
    if(inode->is_hardlink || inode->is_file) {
        perm |= S_IFREG;
    }
    // RGDTODO - Is device/socket not implemented.
    return perm;
}

inode_rtvl_t inode_set_permissions(inode_t *inode, inode_perm_t perm, inode_perm_t umask)
{
   // Set user permissions first.
   int mode = inode_get_mode(perm, umask);
   if(mode == INODE_READ) {
        inode->user_read    = true;
        inode->user_write   = false;
        inode->user_execute = false;
   } else if(mode == INODE_WRITE) {
        inode->user_read    = false;
        inode->user_write   = true;
        inode->user_execute = false;
   } else if(mode == INODE_EXECUTE) {
        inode->user_read    = false;
        inode->user_write   = false;
        inode->user_execute = true;
   } else if(mode == INODE_RW) {
        inode->user_read    = true;
        inode->user_write   = true;
        inode->user_execute = false;
   } else if(mode == INODE_RX) {
        inode->user_read    = true;
        inode->user_write   = false;
        inode->user_execute = true;
   } else if(mode == INODE_WX) {
        inode->user_read    = false;
        inode->user_write   = true;
        inode->user_execute = true;
   } else if(mode == INODE_RWX) {
        inode->user_read    = true;
        inode->user_write   = true;
        inode->user_execute = true;
   } else {
        inode->user_read    = false;
        inode->user_write   = false;
        inode->user_execute = false;
   }
   // Set group permissions second.
   perm <<= 3;
   mode = inode_get_mode(perm, umask);
   if(mode == INODE_READ) {
        inode->group_read    = true;
        inode->group_write   = false;
        inode->group_execute = false;
   } else if(mode == INODE_WRITE) {
        inode->group_read    = false;
        inode->group_write   = true;
        inode->group_execute = false;
   } else if(mode == INODE_EXECUTE) {
        inode->group_read    = false;
        inode->group_write   = false;
        inode->group_execute = true;
   } else if(mode == INODE_RW) {
        inode->group_read    = true;
        inode->group_write   = true;
        inode->group_execute = false;
   } else if(mode == INODE_RX) {
        inode->group_read    = true;
        inode->group_write   = false;
        inode->group_execute = true;
   } else if(mode == INODE_WX) {
        inode->group_read    = false;
        inode->group_write   = true;
        inode->group_execute = true;
   } else if(mode == INODE_RWX) {
        inode->group_read    = true;
        inode->group_write   = true;
        inode->group_execute = true;
   } else {
        inode->group_read    = false;
        inode->group_write   = false;
        inode->group_execute = false;
   }

   // Set world permissions third.
   perm <<= 3;
   mode = inode_get_mode(perm, umask);
   if(mode == INODE_READ) {
        inode->world_read    = true;
        inode->world_write   = false;
        inode->world_execute = false;
   } else if(mode == INODE_WRITE) {
        inode->world_read    = false;
        inode->world_write   = true;
        inode->world_execute = false;
   } else if(mode == INODE_EXECUTE) {
        inode->world_read    = false;
        inode->world_write   = false;
        inode->world_execute = true;
   } else if(mode == INODE_RW) {
        inode->world_read    = true;
        inode->world_write   = true;
        inode->world_execute = false;
   } else if(mode == INODE_RX) {
        inode->world_read    = true;
        inode->world_write   = false;
        inode->world_execute = true;
   } else if(mode == INODE_WX) {
        inode->world_read    = false;
        inode->world_write   = true;
        inode->world_execute = true;
   } else if(mode == INODE_RWX) {
        inode->world_read    = true;
        inode->world_write   = true;
        inode->world_execute = true;
   } else {
        inode->world_read    = false;
        inode->world_write   = false;
        inode->world_execute = false;
   }
   return INODE_OK;
}

inode_mode_t inode_get_mode(inode_perm_t perm,
                            inode_perm_t umask)
{
    // Apply the mask.
    perm |= umask;
    if(S_IRUSR & perm && S_IWUSR & perm && S_IXUSR & perm) {
        return INODE_RWX;
    }
    if((S_IWUSR & perm) && (S_IXUSR & perm)) {
        return INODE_WX;
    }
    if((S_IRUSR & perm) && (S_IXUSR & perm)) {
        return INODE_RX;
    }
    if((S_IRUSR & perm) && (S_IWUSR & perm)) {
        return INODE_RW;
    }
    if(S_IXUSR & perm) {
        return INODE_EXECUTE;
    }
    if(S_IWUSR & perm) {
        return INODE_WRITE;
    }
    if(S_IRUSR & perm) {
        return INODE_READ;
    }
    return INODE_INVALID_MODE;
}

inode_mode_t inode_get_mode_from_all(inode_perm_t perm,
                                     inode_perm_t umask)
{
    int iter = 2;
    // Apply the mask.
    perm |= umask;
    do {
       if(S_IRWXU & perm) {
           return INODE_RWX;
       }
       if((S_IWUSR & perm) && (S_IXUSR & perm)) {
           return INODE_WX;
       }
       if((S_IRUSR & perm) && (S_IXUSR & perm)) {
           return INODE_RX;
       }
       if((S_IRUSR & perm) && (S_IWUSR & perm)) {
           return INODE_RW;
       }
       if(S_IXUSR & perm) {
           return INODE_EXECUTE;
       }
       if(S_IWUSR & perm) {
           return INODE_WRITE;
       }
       if(S_IRUSR & perm) {
           return INODE_READ;
       }
       // We determine permissions also if group or world
       // were set. 
       // RGDTODO: Can we assume that group matches the user group.
       perm <<= 3;
    }
    while(iter--);
    return INODE_INVALID_MODE;
}

// Return true or false if we can perform the operation 
// requested based on the input.
bool inode_check_permissions(inode_t *inode,
                             inode_perm_t perm,  // Not used.
                             inode_perm_t umask, // Not used.
                             inode_group_t group, 
                             inode_own_t owner,
                             inode_mode_t mode)
{
   if(mode == INODE_DONTCHECKPERMS) {
        return true;
   }
   if(mode == INODE_READ) {
        if(owner == inode->owner) {
            if(inode->user_read) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_read) {
                return true;
            }
        }
        if(inode->world_read) {
            return true;
        }
        return false;
   } else if(mode == INODE_WRITE) {
        if(owner == inode->owner) {
            if(inode->user_write) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_write) {
                return true;
            }
        }
        if(inode->world_write) {
            return true;
        }
        return false;
   } else if(mode == INODE_EXECUTE) {
        if(owner == inode->owner) {
            if(inode->user_execute) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_execute) {
                return true;
            }
        }
        if(inode->world_execute) {
            return true;
        }
        return false;
   } else if(mode == INODE_RW) {
        if(owner == inode->owner) {
            if(inode->user_read && inode->user_write) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_read && inode->group_write) {
                return true;
            }
        }
        if(inode->world_read && inode->world_write) {
            return true;
        }
        return false;
   } else if(mode == INODE_RX) {
        if(owner == inode->owner) {
            if(inode->user_read && inode->user_execute) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_read && inode->group_execute) {
                return true;
            }
        }
        if(inode->world_read && inode->world_execute) {
            return true;
        }
        return false;
   } else if(mode == INODE_WX) {
        if(owner == inode->owner) {
            if(inode->user_write && inode->user_execute) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_write && inode->group_execute) {
                return true;
            }
        }
        if(inode->world_write && inode->world_execute) {
            return true;
        }
        return false;
   } else if(mode == INODE_RWX) {
        if(owner == inode->owner) {
            if(inode->user_read && inode->user_write && inode->user_execute) {
                return true;
            }
        } 
        if(group == inode->group) {
            if(inode->group_read && inode->group_write && inode->group_execute) {
                return true;
            }
        }
        if(inode->world_read && inode->world_write && inode->world_execute) {
            return true;
        }
        return false;
   } else {
        printk("inode_check_permissions:: invalid mode=%d\n", mode);
        return false;
   }
}

inode_rtvl_t inode_create(int dev,
                          block_t current_dir,  // cwd
                          char *path,           // path of node to create
                          inode_mode_t  mode,   // mode in which we are to create the node
                          inode_perm_t  perm,   // desired permissions
                          inode_perm_t  umask,  // umask
                          inode_group_t group,  // our group
                          inode_own_t   owner,  // our owner
                          char *lpath)          // for creating sym and hard links
{
       int  start = 0, tmp = 0, i = 0, j = 0, ptr_tab_len = 0;
       char res_path[MAX_PATH]={0}, 
            in_path[MAX_PATH]={0}, 
            link_path[MAX_PATH]={0},
            *ptr_tab[MAX_PATH]={0}; 
       block_map_t bmap={0}, zero_bmap={0};
       block_t block = 0, parent = 0, current = 0, found = 0;
       inode_t inode ={0}, tnode = {0}, parent_inode = {0};
       link_t link={0};
       master_inode_t *master = master_get(dev);

       if(!master) {
            errno = EINVAL;
            printk("inode_create:: error getting master dev=%d\n",dev);
            return INODE_FAIL;
       }
       if(!path || (!lpath && (mode == INODE_CREATE_SYMLINK || mode == INODE_CREATE_HARDLINK))) {
            errno = EINVAL;
            printk("inode_create:: error NULL path\n");
            return INODE_FAIL;
       }
       if(strlen(path) > MAX_PATH) {
            errno = ENAMETOOLONG;
            printk("inode_create:: error path name too long\n");
            return INODE_FAIL;
       }
       if(lpath && strlen(lpath) > MAX_PATH) {
            printk("inode_create:: error link_path name too long\n");
            errno = ENAMETOOLONG;
            return INODE_FAIL; 
       }
       strcpy(in_path,path);
       if(!krealpath(dev, current_dir, path, res_path, &errno)) {
            printk("inode_create:: error resolving absolute path "
                   "dev=%d current_dir=%u path=%s res_path=%s error=%d\n",
                    dev, current_dir, path, res_path, errno);
            return INODE_FAIL;
       }
       if(lpath) {
            // Assert link target is a full path.
            if(!krealpath(dev, current_dir, lpath, link_path, &errno)) {
                printk("inode_create:: error resolving absolute path "
                       "dev=%d current_dir=%u path=%s res_path=%s error=%d\n",
                        dev, current_dir, path, res_path, errno);
                return INODE_FAIL;
            }
       }
       // Parse the path getting every component and computing the
       // number of components.
       while((path=get_next_path(&start, res_path)) != NULL) {
            ptr_tab[ptr_tab_len++] = path;
            start++;
       }
       // Check if we have root inode,
       if(block_read(dev, INODE_ROOT_BLOCK, (char *)&inode) != BLOCK_OK) {
            errno = EACCES;
            printk("inode_get:: error reading block dev=%d root block=%u\n",
                    dev, INODE_ROOT_BLOCK);
            return INODE_FAIL;
       }
       if(strcmp(inode.path,"/")) {
          // Initialize.
          strcpy(inode.path,"/");
          inode.next = INODE_NULL;
          if(inode_set_permissions(&inode, 0777, 0000) != INODE_OK) {
                printk("inode_get:: error setting / permissions to 0777\n");
                errno = EPERM;
                return INODE_FAIL;
          }
          if(inode_set_timestamps(&inode) != INODE_OK) {
              printk("inode_create:: error setting timestamp\n");
              errno = EINVAL;
              return INODE_FAIL;
          }
          inode.dev = dev;
          inode.is_directory = true;
          inode.self = INODE_ROOT_BLOCK;
          inode.parent = INODE_ROOT_BLOCK;
          inode.group = 0;
          inode.owner = 0;
          inode.modified_time = ktime(0);
          if(inode_get_inode_block(dev, &block) != INODE_OK) {
                errno = EACCES;
                printk("inode_get:: error allocating inode block dev=%d block=%u\n",
                        dev, block);
                return INODE_FAIL;
          }
          if(block != INODE_ROOT_BLOCK) {
                printk("inode_get:: error allocating INODE_ROOT_BLOCK=%u block=%u\n",
                        INODE_ROOT_BLOCK,block);
                return INODE_FAIL;
          }
          if(block_write(dev, INODE_ROOT_BLOCK, (char *)&inode) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_get:: error creating root inode dev=%d\n", dev);
                return INODE_FAIL;
          }
       }
       // At this point, we have the root inode loaded.
       // It serves as parent for the whole file system.
       for(i = 1, parent = inode.self; i < ptr_tab_len; ++i) {
            path = ptr_tab[i];
            // Look up the path component in the parent.
            if(block_read(dev, parent, (char *)&inode) != BLOCK_OK) {
                errno = EACCES;
                printk("inode_create:: error reading block dev=%d parent=%u\n",
                        dev, parent);
                return INODE_FAIL;
            }
            // If parent is a directory and we are not at the end of the path.
            // RGDTODO - Check permissions as we consume the path.
            if((inode.is_directory || inode.is_symlink) && (i != (ptr_tab_len-1))) {
                if(inode.is_symlink) {
                    if((block=kreadlink(dev, inode.self, inode.path, &errno)) == INODE_NULL) {
                        printk("inode_create:: kreadlink error dev=%d inode.self=%u path=%s\n",
                                dev, inode.self, inode.path);
                        return INODE_FAIL;
                    }
                    if(block_read(dev, block, (char *)&inode) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_create:: error reading block dev=%d block=%u\n", dev, block);
                        return INODE_FAIL;
                    }
                    if(!inode.is_directory) {
                        printk("inode_create:: error symlink does not resolve to directory "
                               "dev=%d block=%u\n", dev, block);
                        errno = EACCES;
                        return INODE_FAIL;
                    }
                    parent = inode.self;
                }
                if(!inode_check_permissions(&inode, perm, umask, 
                                            current_process->group, 
                                            current_process->owner, INODE_RX)) {
                    errno = EACCES;
                    printk("inode_create:: INODE_RX permissions failed perm=%u umask=%u group=%u owner=%u\n",
                           perm, umask, group, owner);
                    return INODE_FAIL;
                }
                current = bmap.next = inode.next;
                if(inode.next == INODE_NULL) {
                    errno = ENOENT;
                    printk("inode_create:: file not found [%s]\n", in_path);
                    // *res_inode = *zero_node;
                    return INODE_FILE_NOT_FOUND;
                }
                block = INODE_NULL;
                found = INODE_NULL;
                do {
                    current = bmap.next;
                    if(block_read(dev, bmap.next, (char *)&bmap) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_create:: error reading block dev=%d block=%u\n",
                                dev, bmap.next);
                        return INODE_FAIL;
                    }
                    for(j = 0; j < BMAP_BLOCKS; j++) {
                        if(bmap.blocks[j] != INODE_NULL) {
                            if(block_read(dev, bmap.blocks[j], (char *)&tnode) != BLOCK_OK) {
                                errno = EACCES;
                                printk("inode_create:: error reading block dev=%d block=%u\n",
                                        dev, bmap.blocks[j]);
                                return INODE_FAIL;
                            }
                            if(!strcmp(tnode.path,path)) {
                                found = bmap.blocks[j];
                                break;
                            }
                        }
                    }
                } while(bmap.next != INODE_NULL && found == INODE_NULL);
                if(found != INODE_NULL) {
                    // We found it, we were not at the end of the path
                    // so go back in the loop using the found node
                    // as parent.
                    parent = found;
                    continue;
                } else {
                    // We fail here as the path doesn't exist.
                    // Return no entry.
                    errno = ENOENT;
                    return INODE_FILE_NOT_FOUND;
                }
            } else if(!(inode.is_directory || inode.is_symlink) && (i != (ptr_tab_len-1))) {
                if(inode.is_file) {
                    // This is an error, the path contains a file.
                    errno = ENOENT;
                    printk("inode_create:: path contains a file [expected directory]\n");
                    return INODE_FAIL;
                } else {
                    // Can not have a hard link to a directory.
                    printk("inode_create:: path invalid\n");
                    errno = EINVAL;
                    return INODE_FAIL;
                }
            } else if((inode.is_directory || inode.is_symlink) && i == (ptr_tab_len-1)) {
                // We are at the end of the path, check if we exist,
                // error out if we do.
                // scan_code
                if(inode.is_symlink) {
                    if((block=kreadlink(dev, inode.self, inode.path, &errno)) == INODE_NULL) {
                        printk("inode_create:: kreadlink error dev=%d current_dir=%u path=%s\n",
                                dev, current_dir, inode.path);
                        return INODE_FAIL;
                    }
                    if(block_read(dev, block, (char *)&inode) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_create:: error reading block dev=%d block=%u\n", dev, block);
                        return INODE_FAIL;
                    }
                    if(!inode.is_directory) {
                        printk("inode_create:: error symlink does not resolve to directory "
                               "dev=%d block=%u\n", dev, block);
                        errno = EACCES;
                        return INODE_FAIL;
                    }
                    parent = inode.self;
                }
                if(!inode_check_permissions(&inode, perm, umask, 
                                            current_process->group, 
                                            current_process->owner, INODE_RWX)) {
                    errno = EACCES;
                    printk("inode_create:: INODE_RWX permissions failed perm=%u umask=%u group=%u owner=%u\n",
                           perm, umask, group, owner);
                    return INODE_FAIL;
                }
                current = bmap.next = inode.next;
                block = INODE_NULL;
                found = INODE_NULL;
                if(inode.next != INODE_NULL) {
                    do {
                        current = bmap.next;
                        if(block_read(dev, bmap.next, (char *)&bmap) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_create:: error reading block dev=%d block=%u\n",
                                    dev, bmap.next);
                            return INODE_FAIL;
                        }
                        for(j = 0; j < BMAP_BLOCKS; j++) {
                            if(bmap.blocks[j] != INODE_NULL) {
                                if(block_read(dev, bmap.blocks[j], (char *)&tnode) != BLOCK_OK) {
                                    errno = EACCES;
                                    printk("inode_create:: error reading block dev=%d block=%u\n",
                                            dev, bmap.blocks[j]);
                                    return INODE_FAIL;
                                }
                                if(!strcmp(tnode.path,path)) {
                                    found = bmap.blocks[j];
                                    break;
                                }
                            }
                        }
                    } while(bmap.next != INODE_NULL && found == INODE_NULL);
                }
                if(found != INODE_NULL) {
                    // We found it, but this time, we are at the end of
                    // the path. This is an error.
                    errno = EEXIST;
                    printk("inode_create:: path exists at block=%u\n",found);
                    return INODE_FAIL;
                }

                // Create what the caller is asking for.
                // 'found' should be INODE_NULL. However, there are two cases,
                // a case where inode.next was INODE_NULL and one where it
                // was not. These need to be handled differently and
                // we do this using an if statement to check if parent.next is INODE_NULL.
                // create_code
                if(block_read(dev, parent, (char *)&parent_inode) != BLOCK_OK) {
                    errno = EACCES;
                    printk("inode_create:: error reading block dev=%d block=%u\n",
                            dev, parent);
                    return INODE_FAIL;
                }
                current = bmap.next = parent_inode.next;
                block = INODE_NULL;
                if (parent_inode.next != INODE_NULL) {
                    do {
                        current = bmap.next;
                        if(block_read(dev, bmap.next, (char *)&bmap) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_create:: error reading block dev=%d block=%u\n",
                                    dev, bmap.next);
                            return INODE_FAIL;
                        }
                        for(j = 0; j < BMAP_BLOCKS; j++) {
                            if(bmap.blocks[j] == INODE_NULL) {
                                if(inode_get_inode_block(dev, &block) == INODE_FAIL) {
                                    errno = ENOSPC;
                                    printk("inode_create:: failed to get inode block dev=%d\n",
                                            dev);
                                    return INODE_FAIL;
                                }
                                bmap.blocks[j] = block;
                                if(block_write(dev, current, (char *)&bmap) != BLOCK_OK) {
                                    errno = EACCES;
                                    printk("inode_create:: failed to write dev=%d block=%u\n", dev, current);
                                    return INODE_FAIL; 
                                }
                                break;
                            }
                        }
                    } while(bmap.next != INODE_NULL && block == INODE_NULL);
                }
                if(block == INODE_NULL) {
                    // Else, we didn't find a slot in the directory, we need
                    // a new bmap.
                    if(inode_get_data_block(dev, &block) == INODE_FAIL) {
                        errno = ENOSPC;
                        printk("inode_create:: failed to get data block dev=%d\n", dev);
                        return INODE_FAIL;
                    }
                    // This creates the next bmap block.
                    // current should be bmap.next from above search loop.
                    // NOTE: current should be tested to see if it is INODE_NULL, 
                    // if it is, current = block, and we insert at the head.
                    // Otherwise, for current, set its bmap.next to be block.
                    // The new bmap is initialized further down after an inode
                    // has been allocated.
                    if(bmap.next == INODE_NULL && current != INODE_NULL) {
                        // Install at the end.
                        bmap.next = block;
                        if(block_write(dev, current, (char *)&bmap) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, current);
                            return INODE_FAIL;

                        }
                    } else if(current == INODE_NULL) {
                        // Install at beginning. 
                        parent_inode.next = block;
                        current = bmap.next = block;
                        printk("parent_inode.self=%u parent_inode.next=%u\n",
                                parent_inode.self, parent_inode.next);
                        if(block_write(dev, parent_inode.self, (char *)&parent_inode)
                                != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, parent_inode.self);
                            return INODE_FAIL;
                        }
                        /* TEST if the write worked.
                        { inode_t test={0};
                          block_read(dev, parent_inode.self, (char *)&test);
                          if(test.next == INODE_NULL) {
                              printk("ERROR!!!\n");
                              return INODE_FAIL;
                          }
                        }
                        */
                    }
                    // Now allocate an inode.
                    // NOTE: When when we first run, we must set the first inode
                    // as allocated as this is the root, we do it inside inode_get and
                    // inside inode_create.
                    if(inode_get_inode_block(dev, &block) == INODE_FAIL) {
                        errno = ENOSPC;
                        printk("inode_create:: failed to get inode block dev=%d\n", dev);
                        return INODE_FAIL;
                    }
                    // Install the new inode, and write it out.
                    // bmap.next is the new data block to be written out
                    // once the inode is allocated.
                    zero_bmap.blocks[0] = block;
                    if(block_write(dev, bmap.next, (char *)&zero_bmap) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_create:: error writing block dev=%d block=%u\n",
                                dev, current);
                        return INODE_FAIL;
                    }
                    // block refers to the newly allocate inode.
                }
                // At this point, block points to the new inode.
                // Setup dev.
                // create-inode
                inode.dev = dev;
                if(mode == INODE_CREATE_FILE) {
                        memset((char *)&inode,0x0,DEV_BLOCK_SIZE);
                        inode.is_file = true;
                        strcpy(inode.path, path);
                        if(inode_set_permissions(&inode, perm, umask) != INODE_OK) {
                            printk("inode_create:: error setting permissions\n");
                            errno = EPERM;
                            return INODE_FAIL;
                        }
                        if(inode_set_timestamps(&inode) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.self   = block;
                        inode.parent = parent;
                        if(inode_set_parent_mod_time(dev,parent) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.group  = group;
                        inode.owner  = owner;
                        if(block_write(dev, block, (char *)&inode) != BLOCK_OK) {
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, block);
                            errno=EACCES;
                            return INODE_FAIL;
                        }
               } else if(mode == INODE_CREATE_DIR) {
                        memset((char *)&inode,0x0,DEV_BLOCK_SIZE);
                        inode.is_directory = true;
                        strcpy(inode.path, path);
                        if(inode_set_permissions(&inode, perm, umask) != INODE_OK) {
                            printk("inode_create:: error setting permissions\n");
                            errno = EPERM;
                            return INODE_FAIL;
                        }
                        if(inode_set_timestamps(&inode) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.self   = block;
                        inode.parent = parent;
                        if(inode_set_parent_mod_time(dev,parent) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.group  = group;
                        inode.owner  = owner;
                        if(block_write(dev, block, (char *)&inode) != BLOCK_OK) {
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, block);
                            errno=EACCES;
                            return INODE_FAIL;
                        }
                } else if (mode == INODE_CREATE_SYMLINK) {
                        memset((char *)&inode,0x0,DEV_BLOCK_SIZE);
                        inode.is_symlink = true;
                        strcpy(inode.path, path);
                        if(inode_set_permissions(&inode, perm, umask) != INODE_OK) {
                            printk("inode_create:: error setting permissions\n");
                            errno = EPERM;
                            return INODE_FAIL;
                        }
                        if(inode_set_timestamps(&inode) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.self   = block;
                        inode.parent = parent;
                        if(inode_set_parent_mod_time(dev,parent) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.group  = group;
                        inode.owner  = owner;
                        // Create the link;
                        if(inode_get_data_block(dev, &inode.next) != INODE_OK) {
                            printk("inode_create:: error allocated data block dev=%d\n",dev);
                            errno = ENOSPC;
                            return INODE_FAIL;
                        }
                        strcpy(link.path, link_path);
                        printk("inode_create:: inode.next=%u\n", inode.next);
                        if(block_write(dev, inode.next, (char *)&link) != BLOCK_OK) {
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, inode.next);
                            errno = EACCES;
                            return INODE_FAIL;
                        }
                        printk("inode_create:: block=%u\n", block);
                        if(block_write(dev, block, (char *)&inode) != BLOCK_OK) {
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, block);
                            errno=EACCES;
                            return INODE_FAIL;
                        }
                } else if (mode == INODE_CREATE_HARDLINK) {
                        memset((char *)&inode,0x0,DEV_BLOCK_SIZE);
                        inode.is_hardlink = true;
                        strcpy(inode.path, path);
                        if(inode_set_permissions(&inode, perm, umask) != INODE_OK) {
                            printk("inode_create:: error setting permissions\n");
                            errno = EPERM;
                            return INODE_FAIL;
                        }
                        if(inode_set_timestamps(&inode) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.self   = block;
                        inode.parent = parent;
                        if(inode_set_parent_mod_time(dev,parent) != INODE_OK) {
                            printk("inode_create:: error setting timestamp\n");
                            errno = EINVAL;
                            return INODE_FAIL;
                        }
                        inode.group  = group;
                        inode.owner  = owner;
                        // Look-up the link_path and update its reference count.
                        // Make sure we are not pointing to a directory.
                        // Dont read the link for hardlink, should not have a hardlink
                        // to a softlink anyway.
                        if(inode_get(dev, parent, link_path, 
                                     false, INODE_DONTCHECKPERMS, &tnode) != INODE_OK) {
                            printk("inode_create:: failed to resolve inode dev=%d current_dir=%u link_path=%s\n",
                                    dev, parent, link_path);
                            // Let inode_get set up errno.
                            return INODE_FAIL;
                        }
                        // Disallow hard link to directory, symlink, and hardlink.
                        if(tnode.is_directory) {
                            printk("inode_create:: failed to create hardlink to directory [%s]\n",
                                    link_path);
                            errno = EACCES;
                            return INODE_FAIL;
                        }
                        if(tnode.is_symlink) {
                            printk("inode_create:: failed to create hardlink to symlink [%s]\n",
                                    link_path);
                            errno = EACCES;
                            return INODE_FAIL;
                        }
                        if(tnode.is_hardlink) {
                            printk("inode_create:: failed to create hardlink to hardlink [%s]\n",
                                    link_path);
                            errno = EACCES;
                            return INODE_FAIL;
                        }
                        // Increment our reference count.
                        tnode.refcount++;
                        if(block_write(dev, tnode.self, (char *)&tnode) != BLOCK_OK) {
                            printk("inode_create:: failed to write block dev=%d block=%u\n",
                                    dev, tnode.self);
                            errno = EACCES;
                            return INODE_FAIL;
                        }
                        // Create the link;
                        if(inode_get_data_block(dev, &inode.next) != INODE_OK) {
                            printk("inode_create:: error allocated data block dev=%d\n",dev);
                            errno = ENOSPC;
                            return INODE_FAIL;
                        }
                        strcpy(link.path, link_path);
                        if(block_write(dev, inode.next, (char *)&link) != BLOCK_OK) {
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, inode.next);
                            errno = EACCES;
                            return INODE_FAIL;
                        }
                        if(block_write(dev, block, (char *)&inode) != BLOCK_OK) {
                            printk("inode_create:: error writing block dev=%d block=%u\n",
                                    dev, block);
                            errno=EACCES;
                            return INODE_FAIL;
                        }
                } else {
                    printk("inode_create:: invalid mode %d\n", mode);
                    errno = EINVAL;
                    return INODE_FAIL;
                }
            } else if(!(inode.is_directory || inode.is_symlink) && (i == (ptr_tab_len-1))) {
                if(inode.is_file) {
                    // This is an error, the path contains a file.
                    errno = ENOENT;
                    printk("inode_create:: path contains a file [expected directory]\n");
                    return INODE_FAIL;
                } else {
                    // Can not have a hard link to a directory.
                    printk("inode_create:: path invalid\n");
                    errno = EINVAL;
                    return INODE_FAIL;
                }
            }
       }
       return INODE_OK;
}

// 
// TODO - We should call inode free for the inode being unlinked.
//        DONE
//      - We should double check that the removal of blocks from the bmap is
//        correct, I am not sure about the second loop.
//        DONE - There is a use of the inode.blocks here which may
//        not be consistent with the rest, since read/write don't use it
//        we will just use the block_map_t structure.
//
// inode_free:
//
// This algorithm is the same as 'unlink' system call.
// Basically we find the parent inode and find where in the block_map_t
// our inode to be free'd exists, and set it to null.
// We then call 'inode_free_inode_block' to free the inode from
// the inode map. 
// RGDTODO - The data remains on disk, in file.c (read/write)
// we should add code to zero out the data when retrieved.
// RGDTODO - Check reference count before delete on hardlinks.
//         - Double check we can remove soft and hard links.
//         - Double check we can remove a directory (only if inode.next is INODE_NULL)
//           which implies when we free a file or link, we have code to check
//           if the bmap is blank, and therefore we can remove it.
inode_rtvl_t inode_free(int dev, block_t current_dir, char *path, inode_t *newnode)
{
   /* Free an existing inode. */
   block_t     i = 0, j = 0, k = 0, current = 0;
   inode_t      inode = {0};
   inode_t     parent = {0};
   inode_t      tnode = {0};
   block_map_t   bmap = {0}, data = {0};
   link_t        link = {0};
   inode_rtvl_t  rtvl;
   char in_path[MAX_PATH] = {0};
   if(strlen(path) > MAX_PATH) {
      errno = ENAMETOOLONG;
      printk("inode_free:: path too long [%s]\n",path);
      return INODE_FAIL;
   }
   strcpy(in_path, path);
   // NOTE: We call inode_get here with do_readlink set to false.
   // We assume the caller of inode_free wants to free the actual
   // target and not resolve the symlinks and hardlinks prior to free.
   // So if 'symlink' points to /peach and we are called with 'symlink'
   // as an argument we free 'symlink' not /peach. To free
   // /peach a call with /peach as an argument is needed.
   if((rtvl=inode_get(dev,current_dir,path,false,INODE_DONTCHECKPERMS,&inode)) != INODE_OK) {
      printk("inode_free:: inode_get failed dev=%d current_dir=%u parent_path=%s\n",
              dev, current_dir,in_path);
      return rtvl;
   }
   if(!newnode && inode.is_directory && inode.next != INODE_NULL) {
        /* This is an error, can not delete non empty directory. */
        printk("inode_free:: error free'ing non-empty directory [%s]\n", path);
        errno = EACCES;
        return INODE_FAIL;
   }
   if(!newnode && inode.is_file && inode.refcount > 0) {
        // Some hardlink is still pointing to us.
        // Do not remove the file.
        // NOTE: If we do unlink it, we will not be able
        //       to retrieve it.
        errno = EEXIST;
        printk("inode_free:: warning free'ing file refcount=%u\n",
                inode.refcount);
        return INODE_PARAM;
   }
   /* Go to its parent. */
   if(block_read(dev, inode.parent, (char *)&parent) != BLOCK_OK) {
        errno = EACCES;
        printk("inode_free:: block_read failed dev=%d block=%u\n",
                dev, inode.parent);
        return INODE_FAIL;
   }
   if(inode_set_parent_mod_time(dev, parent.self) != INODE_OK) {
       printk("inode_free:: error setting timestamp\n");
       errno = EINVAL;
       return INODE_FAIL;
   }
   /* Scan for the entry referring to this inode. */
   /* We have to read from the bmap. */
   bmap.next = parent.next;
   do {
      current = bmap.next;
      if(block_read(dev,bmap.next,(char *)&bmap) != BLOCK_OK) {
        errno = EACCES;
        printk("inode_free:: block_read failed dev=%d block=%u\n",
                dev, inode.parent);
        return INODE_FAIL;
      }
      for(i = 0; i < BMAP_BLOCKS; i++) {
         if(inode.self == bmap.blocks[i]) {
            // This is the actual delete logic,
            // find the entry in the list, and set it to
            // INODE_NULL. Then write the block_map back
            // to the disk to record the change and release
            // the inode.
            bmap.blocks[i] = INODE_NULL;
            if(inode_free_inode_block(dev, inode.self) != INODE_OK) {
                printk("inode_free:: error free'ing inode block dev=%d block=%u\n",dev,inode.self);
                return INODE_FAIL;
            }
            if(block_write(dev, current, (char *)&bmap) != BLOCK_OK) {
                printk("inode_free:: error writing dev=%d block=%u\n",
                        dev, current);
                return INODE_FAIL;
            }
            // We are not done yet.
            // Free all data blocks if we are a file/hard/soft link.
            // If we are a hard link, decrement what the reference count we
            // were pointing to.
            if(inode.is_file) {
                if(newnode) {
                    // Copy into the newnode this is
                    // for implementing the rename system call
                    // where newnode is hopefully already allocated using
                    // inode_create and retrieved using inode_get.
                    char tmp[MAX_PATH]={0};
                    block_t self   = newnode->self;
                    block_t parent = newnode->parent;
                    strncpy(tmp, newnode->path, MAX_PATH);
                    // Struct assign.
                    (*newnode) = inode;
                    strncpy(newnode->path,tmp, MAX_PATH);
                    newnode->self   = self;
                    newnode->parent = parent;
                    //P();
                    //printk("inode.next=%u size=%u pos=%u\n",inode.next,inode.size,inode.pos);
                    //printk("newnode->next=%u size=%u pos=%u\n",newnode->next,newnode->size,newnode->pos);
                } else {
                  for(j = inode.next; j != INODE_NULL; j = data.next) {
                    if(block_read(dev, j, (char *)&data) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_free:: error free'ing file [%s]\n", in_path);
                        return INODE_FAIL;
                    }
                    // Free all data referenced in the block_map.
                    for(k = 0; k < BMAP_BLOCKS; k++) {
                        if(data.blocks[k] != INODE_NULL) {
                            if(inode_free_data_block(dev, data.blocks[k]) != INODE_OK) {
                                errno = EACCES;
                                printk("inode_free:: error free'ing file [%s] block=%u\n",
                                       in_path, data.blocks[k]);
                                return INODE_FAIL;
                            }
                        }
                    }
                    // Free the block_map.
                    if(inode_free_data_block(dev, j) != INODE_OK) {
                        errno = EACCES;
                        printk("inode_free:: error free'ing file [%s] block=%u\n",
                                in_path, j);
                        return INODE_FAIL;
                    }
                    // Memory pointed to by data is still valid,
                    // so data.next should still work to set up j
                    // for next iteration.
                  }
                }
            } else if(inode.is_directory) {
                // In this case, inode.next must necessarily be INODE_NULL
                // so there is no data blocks to free. We already free'd
                // the inode above so nothing to do provided we maintain
                // the inode list inside the directory.
                if(newnode) {
                    // Copy into the newnode this is
                    // for implementing the rename system call
                    // where newnode is hopefully already allocated using
                    // inode_create and retrieved using inode_get.
                    char tmp[MAX_PATH]={0};
                    block_t self   = newnode->self;
                    block_t parent = newnode->parent;
                    strncpy(tmp, newnode->path, MAX_PATH);
                    // Struct assign.
                    (*newnode) = inode;
                    strncpy(newnode->path,tmp, MAX_PATH);
                    newnode->self   = self;
                    newnode->parent = parent;
                }
            } else if(inode.is_symlink) {
                // Free the link_t structure associated with the inode.
                if(newnode) {
                    // Copy into the newnode this is
                    // for implementing the rename system call
                    // where newnode is hopefully already allocated using
                    // inode_create and retrieved using inode_get.
                    char tmp[MAX_PATH]={0};
                    block_t self   = newnode->self;
                    block_t parent = newnode->parent;
                    strncpy(tmp, newnode->path, MAX_PATH);
                    // Struct assign.
                    (*newnode) = inode;
                    strncpy(newnode->path,tmp, MAX_PATH);
                    newnode->self   = self;
                    newnode->parent = parent;
                } else {
                    if(inode.next != INODE_NULL) {
                        if(inode_free_data_block(dev, inode.next) != INODE_OK) {
                            errno = EACCES;
                            printk("inode_free:: error free'ing softlink [%s] block=%u\n",
                                in_path, inode.next);
                            return INODE_FAIL;
                        }
                    }
                }
            } else if(inode.is_hardlink) {
                // We must read the link.
                if(newnode) {
                    // Copy into the newnode this is
                    // for implementing the rename system call
                    // where newnode is hopefully already allocated using
                    // inode_create and retrieved using inode_get.
                    char tmp[MAX_PATH]={0};
                    block_t self   = newnode->self;
                    block_t parent = newnode->parent;
                    strncpy(tmp, newnode->path, MAX_PATH);
                    // Struct assign.
                    (*newnode) = inode;
                    strncpy(newnode->path,tmp, MAX_PATH);
                    newnode->self   = self;
                    newnode->parent = parent;
                } else {
                  if(inode.next != INODE_NULL) {
                    if(block_read(dev, inode.next, (char *)&link) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_free:: error free'ing hardlink [%s] block=%u\n",
                                in_path, inode.next);
                        return INODE_FAIL;
                    }
                    /* Dont follow through if we have a hard link to a soft link. */
                    if(inode_get(dev,current_dir,link.path,false,INODE_RW, &tnode) != INODE_OK) {
                        errno = EACCES;
                        printk("inode_free:: error free'ing hardlink [%s] path=%s\n",
                                in_path, link.path);
                        return INODE_FAIL;
                    }
                    tnode.refcount--;
                    if(block_write(dev, tnode.self, (char *)&tnode) != BLOCK_OK) {
                        errno = EACCES;
                        printk("inode_free:: error free'ing hardlink [%s] block=%u\n",
                            in_path, tnode.self);
                        return INODE_FAIL;
                    }
                  } else {
                    errno = EACCES;
                    printk("inode_free:: error free'ing hardlink [%s] block=%u\n",
                            in_path, INODE_NULL);
                    return INODE_FAIL;
                  }
                }
            }
            {// Fixup parent directory block_map.
              int i = 0;
              bool empty = true;
              block_t current = 0, prev = 0;
              block_map_t pnode={0};
              for(prev = current = parent.next; current != INODE_NULL; ) {
                 if(block_read(dev, current, (char *)&bmap) != BLOCK_OK) {
                    errno = EACCES;
                    printk("inode_free:: error reading block dev=%d block=%u\n",
                            dev, current);
                    return INODE_FAIL;
                 }
                 empty = true;
                 for(i = 0; i < BMAP_BLOCKS; ++i) {
                     if(bmap.blocks[i] != INODE_NULL) {
                         empty = false;
                     }
                 }
                 if(empty) {
                     if(prev == current && bmap.next != INODE_NULL) {
                         // We are at the front of the list, set the list head
                         // which is inode.next to current pointer next (bmap.next)
                         // then write inode back to disk at the end of this code block
                         // (see below).
                         parent.next == bmap.next;
                         if(inode_free_data_block(dev, current) != INODE_OK) {
                            errno = EACCES;
                            printk("inode_free:: error free'ing data block dev=%d block=%u\n",
                                    dev, current);
                            return INODE_FAIL;
                         }
                     } else if(prev == current && bmap.next == INODE_NULL) {
                         // We are at the front of the list, set the list head
                         // which is inode.next to null (write inode back to disk
                         // at the end of this code block (see below)).
                         parent.next = INODE_NULL;
                         if(inode_free_data_block(dev, current) != INODE_OK) {
                            errno = EACCES;
                            printk("inode_free:: error free'ing data block dev=%d block=%u\n",
                                    dev, current);
                            return INODE_FAIL;
                         }
                     } else if(prev != current && bmap.next != INODE_NULL) {
                         // Read the prior block_map node and set its next pointer
                         // to bmap.next (which is current pointer next) and delete current.
                         if(block_read(dev, prev, (char *)&pnode) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_free:: error reading block dev=%d block=%u\n",
                                    dev, prev);
                            return INODE_FAIL;
                         }
                         pnode.next = bmap.next;
                         if(block_write(dev, prev, (char *)&pnode) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_free:: error writing block dev=%d block=%u\n",
                                    dev, prev);
                            return INODE_FAIL;
                         }
                         if(inode_free_data_block(dev, current) != INODE_OK) {
                            errno = EACCES;
                            printk("inode_free:: error free'ing data block dev=%d block=%u\n",
                                    dev, current);
                            return INODE_FAIL;
                         }
                     } else if(prev != current && bmap.next == INODE_NULL) {
                         // Read the prior block_map node and set its next pointer
                         // to point to null as we delete curent.
                         if(block_read(dev, prev, (char *)&pnode) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_free:: error reading block dev=%d block=%u\n",
                                    dev, prev);
                            return INODE_FAIL;
                         }
                         pnode.next = INODE_NULL;
                         if(block_write(dev, prev, (char *)&pnode) != BLOCK_OK) {
                            errno = EACCES;
                            printk("inode_free:: error writing block dev=%d block=%u\n",
                                    dev, prev);
                            return INODE_FAIL;
                         }
                         if(inode_free_data_block(dev, current) != INODE_OK) {
                            errno = EACCES;
                            printk("inode_free:: error free'ing data block dev=%d block=%u\n",
                                    dev, current);
                            return INODE_FAIL;
                         }
                     }
                     if(block_write(dev, parent.self, (char *)&parent) != BLOCK_OK) {
                         errno = EACCES;
                         printk("inode_free:: error writing parent dir dev=%d parent_dir=%u\n",
                                dev, parent.self);
                         return INODE_FAIL;
                     }
                     // Can logically only have one block to remove
                     // if we delete one thing at a time.
                     break;
                 }
                 // Advance the pointers.
                 prev = current;
                 current = bmap.next;
              }
           }
           return INODE_OK;
         }
      }
   } while(bmap.next != INODE_NULL);
   /* We didn't find an entry in the parent,
    * return an error.
    */
   return INODE_INCONSISTENT;
}

block_t inode_get_block_number(int dev, block_t current_dir, char *path)
{
    inode_t inode;
    inode_get(dev, current_dir, path, true, INODE_DONTCHECKPERMS, &inode);
    return inode.self;
}

block_t inode_get_parent_block_number(int dev, block_t current_dir, char *path)
{
    inode_t inode;
    inode_get(dev, current_dir, path, true, INODE_DONTCHECKPERMS, &inode);
    return inode.parent;
}

#ifdef _TEST_INODE
int
main(int argc, char **argv)
{
   inode_t node = {0};
   int dev = 0;
   master_inode_t *master = NULL;
   // Obtains the device based on block_open -> dev_open
   // Initialization here is similar to what would happen
   // in the real kernel. We must make the file system first.
   if(inode_dev_open("./inode.dat",&dev) == INODE_INIT) {
       P();
       // NOTE: We start with 4 Megs, 2 Megs are reserved
       // for boot loader and kernel, these are in 0-4095
       // followed by master record in block 4096, the rest
       // is the inode and block map followed by inodes followed
       // by data blocks.
       // We expect a file system inode.dat of size
       // 2518016 where two megs plus master record are
       // 2097664. Subtracting this, we have 420352 of
       // file system overhead which is 1 block for imap
       // and 1 block for bmap followed by 819 inode blocks
       // followed by 3275 data blocks. Overhead is
       // 420352 (1*512+1*512+819*512).
       // One block can hold 512*8 bits representing whether or not
       // an inode or data block is free. We have TWOMEG-1 (less master)
       // to be allocated for inode and data where we have a formula
       // of 20 percent inode 80 percent data thus giving 819 inodes
       // and 3276 data blocks and the need for 1 block to represent each.
       // See inode_mkfs for details on calculations.
       if(inode_mkfs("./inode.dat", 2 * TWOMEG) != INODE_OK) {
            printk("fs_init:: error initializing filesystem\n");
            return 1;
       }
       P();
       if(inode_dev_open("./inode.dat", &dev) != INODE_OK) {
            printk("fs_init:: error opening device\n");
            return 1;
       }
   }
   P();
   // TODO - call inode_create then inode_get
   //      - develop various cases of inode_create/inode_get
   //        and see how it works. Expect many bugs in this area
   //        but hopefully no major structural errors.
   master = master_get(dev);
   master_set_dev("./inode.dat", dev);
   P();
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test", 
                   INODE_CREATE_DIR,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test", 
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   if(inode_get(dev, INODE_ROOT_BLOCK, "/", 
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo", 
                   INODE_CREATE_DIR,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo", 
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);

   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo/bar", 
                   INODE_CREATE_FILE,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/bar", 
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   //
   // Test sym/hard links.
   // Get sym/hard links with and without true or false flag
   // this will also test kreadlink.
   //
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo/symlink", 
                   INODE_CREATE_SYMLINK,0777,0777,0,0,"/peach") != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_create(dev, INODE_ROOT_BLOCK, "/peach", 
                   INODE_CREATE_FILE,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/peach",
                true, INODE_RW, &node) != INODE_OK) { // true => readlink
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/symlink",
                false, INODE_RW, &node) != INODE_OK) { // false => get link itself
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/symlink",
                true, INODE_RW, &node) != INODE_OK) { // true => readlink
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   //
   // Create the pointee file for the hardlink.
   //
   if(inode_create(dev, INODE_ROOT_BLOCK, "/peachtree", 
                   INODE_CREATE_FILE,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   // Hardlink to existing file is the only hardlink permitted.
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo/hardlink",
                   INODE_CREATE_HARDLINK,0777,0777,0,0,"/peachtree") != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/peachtree",
                true, INODE_RW, &node) != INODE_OK) { // true => readlink
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/hardlink",
                false, INODE_RW, &node) != INODE_OK) { // false => get link itself
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/hardlink",
                true, INODE_RW, &node) != INODE_OK) { // true => readlink
        printk("fs_init:: error getting inode\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   //
   // Test inode_free.
   // case 1. Free an existing file => Expect success.
   // case 2. Free a non-existing file => Expect failure.
   // case 3. Free a full directory => Expect failure.
   // case 4. Free an empty directory => Expect success.
   // case 5. Free a symlink => Expect success.
   // case 6. Free a hardlink => Expect success and ref count of pointee to decrease.
   // case 7. Re-create a deleted file => Expect success (test by calling inode_get).
   // case 8. Re-create a directory => Expect success.
   // case 9. Re-create a hardlink => Expect success (call inode_get and check ref count).
   // case 10. Test newnode (which is used to do rename).

   //
   // case 1.
   //
   printk("case 1\n");
   if(inode_free(dev, INODE_ROOT_BLOCK, "/peach", NULL) != INODE_OK) {
        printk("fs_init:: error free'ing /peach\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/peach",
                true, INODE_RW, &node) != INODE_OK) { // true => readlink
        printk("fs_init:: successfully free'd /peach\n");
   }
   //
   // case 2.
   //
   printk("case 2\n");
   if(inode_free(dev, INODE_ROOT_BLOCK, "/peach", NULL) != INODE_OK) {
        printk("fs_init:: successfully failed to free non-existant file\n");
   }
   //
   // case 3.
   //
   printk("case 3\n");
   if(inode_free(dev, INODE_ROOT_BLOCK, "/", NULL) != INODE_OK) {
        printk("fs_init:: successfully failed to free full directory [/]\n");
   }
   //
   // case 4.
   //
   printk("case 4\n");
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo1", 
                   INODE_CREATE_DIR,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_free(dev, INODE_ROOT_BLOCK, "/test/foo1", NULL) != INODE_OK) {
        printk("fs_init:: error free'ing empty directory\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo1", 
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: successfully free'd /test/foo1\n");
   }
   //
   // case 5.
   //
   printk("case 5\n");
   if(inode_free(dev, INODE_ROOT_BLOCK, "/test/foo/symlink", NULL) != INODE_OK) {
        printk("fs_init:: error free'ing symlink\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/symlink",
                false, INODE_RW, &node) != INODE_OK) { // don't readlink.
        printk("fs_init:: successfully free'd /test/foo/symlink\n");
   } else {
        printk("fs_init:: error free'ing symlink\n");
   }
   //
   // case 6.
   //
   printk("case 6\n");
   if(inode_free(dev, INODE_ROOT_BLOCK, "/test/foo/hardlink", NULL) != INODE_OK) {
        printk("fs_init:: error free'ing hardlink\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/hardlink",
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: successfully free'd /test/foo/hardlink\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/peachtree",
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting file /peachtree\n");
   }
   if(node.refcount != 0) {
        printk("fs_init:: error decrementing peachtree refcount\n");
   } else {
        printk("fs_init:: successfully decremented peachtree refcount\n");
   }
   //
   // case 7.
   //
   printk("case 7\n");
   if(inode_create(dev, INODE_ROOT_BLOCK, "/peach", 
                   INODE_CREATE_FILE,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/peach",
                true, INODE_RW, &node) == INODE_OK) { // true => readlink
        printk("fs_init:: successfully created /peach\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   //
   // case 8.
   //
   printk("case 8\n");
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo1", 
                   INODE_CREATE_DIR,0777,0777,0,0,NULL) != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo1", 
                true, INODE_RW, &node) == INODE_OK) {
        printk("fs_init:: successfully created /test/foo1\n");
   }
   printk("fs_init:: node.path=%s\n",node.path);
   //
   // case 9.
   //
   // Hardlink to existing file is the only hardlink permitted.
   printk("case 9\n");
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo/hardlink",
                   INODE_CREATE_HARDLINK,0777,0777,0,0,"/peachtree") != INODE_OK) {
        printk("fs_init:: error creating inode\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/peachtree",
                true, INODE_RW, &node) != INODE_OK) { // true => readlink
        printk("fs_init:: error getting inode\n");
   }
   if(node.refcount != 1 || strcmp(node.path,"peachtree")) {
        printk("fs_init:: error creating hardlink\n");
   } else {
        printk("fs_init:: successfully recreated hardlink\n");
   }
   //
   // case 10.
   //
   printk("case 10\n");
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo/symlink", 
                   INODE_CREATE_SYMLINK,0777,0777,0,0,"/peach") != INODE_OK) {
        printk("fs_init:: error creating inode /test/foo/symlink\n");
   }
   if(inode_create(dev, INODE_ROOT_BLOCK, "/test/foo/symlink1", 
                   INODE_CREATE_SYMLINK,0777,0777,0,0,"/") != INODE_OK) {
        printk("fs_init:: error creating inode /test/foo/symlink1\n");
   }
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/symlink1",
                false, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting file /test/foo/symlink1\n");
   }
   // Assign symlink to symlink1 on free.
   if(inode_free(dev, INODE_ROOT_BLOCK, "/test/foo/symlink", &node) != INODE_OK) {
        printk("fs_init:: error free'ing hardlink\n");
   }
   // This is needed as no write of node was done (see file.c/rename).
   if(block_write(dev, node.self, (char *)&node) != BLOCK_OK) {
        printk("error writing block dev=%d block=%u\n", dev, node.self);
   }
   // Resolve it to whatever was in symlink (i.e. /peach).
   if(inode_get(dev, INODE_ROOT_BLOCK, "/test/foo/symlink1",
                true, INODE_RW, &node) != INODE_OK) {
        printk("fs_init:: error getting file /test/foo/symlink1\n");
   }
   // Test to see that it is /peach...
   if(!strcmp(node.path,"peach")) {
        printk("fs_init:: successfully renamed link\n");
   } else {
        printk("fs_init:: error renaming link\n");
   }
   printk("end tests...\n");
   //
   // End of tests.
   // NOTE: inode_free of file with data contents has not yet
   // been tested. This will be tested once we start testing file.c.
   //
   if(inode_dev_close(dev) != INODE_OK) {
        printk("fs_init:: error closing device\n");
   }
   P();
   return 0;
}
#endif
