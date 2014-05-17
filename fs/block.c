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
//      block.c
//
// @description:
//      Buffer cache storing disk read/writes in RAM for
//      optimization. The cache supports multiple devices
//      as indicated by an 'int dev' parameter as well
//      as LRU swaps of blocks when the cache is full.
//      It does not use dynamic memory. It is intended
//      to interface directly low-level device read/writes.
//      Initially these devices will be floppy disk and
//      hard disk.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#include "block.h"
#include "dev.h"
#include "bool.h"

#ifdef _TEST_BLOCK_INC
#include <string.h>
#include <stdio.h>
#define printk printf
#else
#include <ox/error_rpt.h>
#endif

// TODO - Figure out what ox kernel headers are needed.

static bool init = false;
static block_t  curr = 0;
static block_t highest = 0; // Highest LRU value.
static bool dirty[BLOCK_ARRAY_SIZE]; // true if the associated block needs write.
static block_t block_lru[BLOCK_ARRAY_SIZE]; // Count to implement LRU algorithm.
static block_t  block_map[BLOCK_ARRAY_SIZE]; // Either it's BLOCK_FREE or the actual block number.
static char block_array[BLOCK_ARRAY_SIZE][DEV_BLOCK_SIZE]; // The cache.
static int  block_dev[BLOCK_ARRAY_SIZE]; // Device the block is on.

block_rtvl_t block_open(char *path, int *dev)
{
   register block_t i = 0;
   if(!path || !dev) {
      return BLOCK_PARAM;
   }
   if(!init) {
      printk("block_open:: initializing buffer cache\n");
      for(i = 0; i < BLOCK_ARRAY_SIZE; i++) {
         // Initialize each block which is of size 'DEV_BLOCK_SIZE'
         // which in turn must be the size of the disk sector read/write
         // historically 512 bytes. New hard drives may now support 4096 bytes.
         memset(block_array[i],0x0,DEV_BLOCK_SIZE);
         dirty[i] = false;
         block_map[i] = BLOCK_FREE;
         block_lru[i] = 0;
         block_dev[i] = DEV_NODEV;
      }
      init = true;
   }
   // TODO - Open the device. In user space, a file, in kernel
   // this must initialize the driver to read from the specified
   // device. The path should allow us to lookup a hard disk
   // and initialize the driver for it. We therefore need a table
   // to help us map say '/' with the main hard disk.
   if(dev_open(path, dev) == DEV_OK) {
      dev_scan(*dev, 0);
      return BLOCK_OK;
   } else {
      // TODO - Kernel should panic here or at least tell us
      //        that the device failed.
      return BLOCK_FAIL;
   }
}

block_rtvl_t block_read(int dev, block_t block, char *data)
{
   register block_t i = 0,
            found = BLOCK_NOPOS,
            free_block_ptr = BLOCK_NOPOS,
            lowest_clean_block = BLOCK_MAX_LRU, lowest_clean_block_ptr = BLOCK_NOPOS,
            lowest_dirty_block = BLOCK_MAX_LRU, lowest_dirty_block_ptr = BLOCK_NOPOS;

   if(dev < 0 || block < 0 || !data) {
      return BLOCK_PARAM;
   }
   // Scan the cache to see if the block is there.

   // Current is a pointer into the cache. It is a slight
   // optimization as we will allocate from current if possible.
   if(curr >= BLOCK_ARRAY_SIZE) {
      curr = 0;
   }

   for(i = curr; i < BLOCK_ARRAY_SIZE; i++) {
        if(block_map[i] == block && block_dev[i] == dev) {
            // Found the desired block, we are done.
            printk("found line %d\n",__LINE__);
            found = i;
            break;
        }
   }
   // We may have to go around again...
   if(curr && found == BLOCK_NOPOS) {
        for(i = 0; i < curr; i++) {
            if(block_map[i] == block && block_dev[i] == dev) {
                // Found the desired block, we are done.
                curr = i;
                found = i;
                printk("found line %d\n",__LINE__);
                break;
            }
        }
   }
        
   // Here we found the block in the cache.
   if(found != BLOCK_NOPOS) {
         i = found;
         // Found, return a copy of it.
         memcpy(data,block_array[i],DEV_BLOCK_SIZE);
         // Increment the count.
         if(block_lru[i] < BLOCK_MAX_LRU) {
            ++block_lru[i];
         }
         curr = i;
         printk("found line %d\n",__LINE__);
         return BLOCK_OK;
   }

   // We scan to see if there is a free block in case its not there.
   // We scan for the lowest LRU block that is not dirty.
   // We scan for the lowest LRU block that is dirty.
   // The rest of the code handles the cases as it is possible that
   // there is no match for one of the cases but not all. 
   // NOTE: This is an optimization, the cache
   // as written requires a scan to find a record, we include our
   // checks in one scan so as to not do many scans.
   for(i = 0; i < BLOCK_ARRAY_SIZE; i++) {
        if(block_map[i] == BLOCK_FREE) {
            free_block_ptr = i;
            break;
        }
        if(!dirty[i]) {
            if(block_lru[i] < lowest_clean_block) {
                lowest_clean_block = block_lru[i];
                lowest_clean_block_ptr = i;
            }
        }
        if(dirty[i]) {
            if(block_lru[i] < lowest_dirty_block) {
                lowest_dirty_block = block_lru[i];
                lowest_dirty_block_ptr = i;
            }
        }
        if(block_lru[i] > highest) {
            highest = block_lru[i];
        }
   }

   // Found a free block.
   if(free_block_ptr != BLOCK_NOPOS) {
         i = free_block_ptr;
         if(dev_scan(dev,block) != DEV_OK) {
            return BLOCK_FAIL; 
         }
         printk("dev_read line %d block %d\n",__LINE__, block);
         if(dev_read(dev, block, block_array[i]) != DEV_OK) {
            block_map[i] = BLOCK_FREE;
            return BLOCK_FAIL;
         }
         if(block_lru[i] < BLOCK_MAX_LRU) {
            block_lru[i] = ++highest;
         }
         block_map[i] = block;
         block_dev[i] = dev;
         dirty[i] = false;
         memcpy(data,block_array[i],DEV_BLOCK_SIZE);
         curr = i;
         printk("block_ok line %d\n",__LINE__);
         return BLOCK_OK;
   }

   /* We have a request and there are no more buffers
    * to store it, we have to free one up.
    * Here we free the block with the lowest LRU that is not
    * dirty.
    */
   if(lowest_clean_block_ptr != BLOCK_NOPOS) {
         i = lowest_clean_block_ptr;
         if(dev_scan(dev, block) != DEV_OK) {
            return BLOCK_FAIL;   
         }
         printk("dev_read line %d block %d\n",__LINE__, block);
         if(dev_read(dev, block, block_array[i]) != DEV_OK) {
            return BLOCK_FAIL;
         }
         if(block_lru[i] < BLOCK_MAX_LRU) {
            block_lru[i] = ++highest;
         }
         block_map[i] = block;
         block_dev[i] = dev;
         memcpy(data,block_array[i],DEV_BLOCK_SIZE);
         dirty[i] = false;
         curr = i;
         return BLOCK_OK;
  }

  // Here we free the lowest LRU block that is dirty.
  // This requires us to write the block that was in the cache first,
  // then read into its position the requested buffer.
  if(lowest_dirty_block_ptr != BLOCK_NOPOS) {
         i = lowest_dirty_block_ptr;
         // Care must be taken to write out to the existing pages device
         // as this may differ from the user supplied 'dev'.
         if(dev_scan(block_dev[i],block_map[i]) != DEV_OK) {
            return BLOCK_FAIL;
         }
         if(dev_write(block_dev[i], block, block_array[i]) != DEV_OK) {
            return BLOCK_FAIL;
         }
         if(dev_scan(dev, block) != DEV_OK) {
            return BLOCK_FAIL;   
         }
         printk("dev_read line %d block %d\n",__LINE__, block);
         if(dev_read(dev, block, block_array[i]) != DEV_OK) {
            return BLOCK_FAIL;
         }
         if(block_lru[i] < BLOCK_MAX_LRU) {
            block_lru[i] = ++highest;
         }
         block_map[i] = block;
         block_dev[i] = dev;
         memcpy(data,block_array[i],DEV_BLOCK_SIZE);
         curr = i;
         dirty[i] = false;
         return BLOCK_OK;
   }
   // If we got here the cached read has failed.
   // This is logically not possible.
   return BLOCK_FAIL;
}

block_rtvl_t block_write(int dev, block_t block, char *data)
{
   register block_t i = 0,
            free_block_ptr = BLOCK_NOPOS,
            lowest_clean_block = BLOCK_MAX_LRU, lowest_clean_block_ptr = BLOCK_NOPOS,
            lowest_dirty_block = BLOCK_MAX_LRU, lowest_dirty_block_ptr = BLOCK_NOPOS;

   if(curr >= BLOCK_ARRAY_SIZE) {
      curr = 0;
   }
   for(i = curr; i < BLOCK_ARRAY_SIZE; i++) {
      if(block_map[i] == block && block_dev[i] == dev) {
         curr = i;
         dirty[i] = true;
         if(block_lru[i] < BLOCK_MAX_LRU) {
            ++block_lru[i];
         }
         memcpy(block_array[i],data,DEV_BLOCK_SIZE);
         return BLOCK_OK;
      }
   }
   if(curr) {
        for(i = 0; i < curr; i++) {
            if(block_map[i] == block && block_dev[i] == dev) {
                curr = i;
                dirty[i] = true;
                if(block_lru[i] < BLOCK_MAX_LRU) {
                    ++block_lru[i];
                }
                memcpy(block_array[i],data,DEV_BLOCK_SIZE);
                return BLOCK_OK;
            }
        }
   }

   // In the above, we scanned to see if it is in the cache,
   // if it is not, we do our block replacement algorithm which
   // should be the same as block_read.

   // We scan to see if there is a free block in case its not there.
   // We scan for the lowest LRU block that is not dirty.
   // We scan for the lowest LRU block that is dirty.
   // The rest of the code handles the cases as it is possible that
   // there is no match for one of the cases but not all. 
   // NOTE: This is an optimization, the cache
   // as written requires a scan to find a record, we include our
   // checks in one scan so as to not do many scans.
   for(i = 0; i < BLOCK_ARRAY_SIZE; i++) {
        if(block_map[i] == BLOCK_FREE) {
            free_block_ptr = i;
            break;
        }
        if(!dirty[i]) {
            if(block_lru[i] < lowest_clean_block) {
                lowest_clean_block = block_lru[i];
                lowest_clean_block_ptr = i;
            }
        }
        if(dirty[i]) {
            if(block_lru[i] < lowest_dirty_block) {
                lowest_dirty_block = block_lru[i];
                lowest_dirty_block_ptr = i;
            }
        }
        if(block_lru[i] > highest) {
            highest = block_lru[i];
        }
   }

   if(free_block_ptr != BLOCK_NOPOS) {
         i = free_block_ptr;
         block_map[i] = block;
         block_dev[i] = dev;
         dirty[i] = true;
         if(block_lru[i] < BLOCK_MAX_LRU) {
            block_lru[i] = ++highest;
         }
         memcpy(block_array[i],data,DEV_BLOCK_SIZE); 
         curr = i;
         return BLOCK_OK;
   }

   /* We have a request and there are no more buffers
    * to store it, we have to free one up.
    * Here we free the block with the lowest LRU that is not
    * dirty.
    */
   if(lowest_clean_block_ptr != BLOCK_NOPOS) {
         i = lowest_clean_block_ptr;
         block_map[i] = block;
         block_dev[i] = dev;
         memcpy(block_array[i],data,DEV_BLOCK_SIZE);
         dirty[i] = true;
         if(block_lru[i] < BLOCK_MAX_LRU) {
            block_lru[i] = ++highest;
         }
         curr = i;
         return BLOCK_OK;
   }
   // Here we free the lowest LRU block that is dirty.
   // This requires us to write the block that was in the cache first,
   // then write into its position the requested buffer.
   if(lowest_dirty_block_ptr != BLOCK_NOPOS) {
         i  = lowest_dirty_block_ptr;
         // Care must be taken to write out to the existing pages device
         // as this may differ from the user supplied 'dev'.
         if(dev_scan(block_dev[i],block_map[i]) != DEV_OK) {
            return BLOCK_FAIL;
         }
         if(dev_write(block_dev[i], block, block_array[i]) != DEV_OK) {
            return BLOCK_FAIL;
         }
         block_map[i] = block;
         block_dev[i] = dev;
         memcpy(block_array[i],data,DEV_BLOCK_SIZE);
         curr = i;
         dirty[i] = true;
         if(block_lru[i] < BLOCK_MAX_LRU) {
            block_lru[i] = ++highest;
         }
         return BLOCK_OK;
   }
   return BLOCK_FAIL; 
}

block_rtvl_t block_free(int dev, block_t block)
{
   register block_t i = 0;

   if(dev < 0 || block < 0) {
      return BLOCK_PARAM;
   }
   for(i = 0; i < BLOCK_ARRAY_SIZE; i++) {
      /* We assume that if the block is not in the cache, 
       * that it was written to disk already.
       */
      if(block_map[i] == block && block_dev[i] == dev) {
         if(dirty[i]) {
            if(dev_scan(dev, block) != DEV_OK) {
                return BLOCK_FAIL;
            }
            if(dev_write(dev, block, block_array[i]) != DEV_OK) {
                return BLOCK_FAIL;
            }
         }
         block_lru[i] = 0;
         block_map[i] = BLOCK_FREE;
         block_dev[i] = DEV_NODEV;
         dirty[i] = false;
         curr = i;
         return BLOCK_OK;
      }
   }
   return BLOCK_OK;
}

block_rtvl_t block_sync(int dev)
{
    register block_t i = 0;
    register block_rtvl_t rtvl = BLOCK_OK;
    for(i = 0; i < BLOCK_ARRAY_SIZE; i++) {
        // Write out all dirty blocks for the specified device.
        if(dirty[i] && block_dev[i] == dev) {
            if(block_disk_write(dev, block_map[i], block_array[i]) == BLOCK_FAIL) {
                printk("block_sync:: error writing block [%d]\n", block_map[i]);
                rtvl = BLOCK_FAIL;
            } else {
                dirty[i] = false;
                if(block_lru[i] < BLOCK_MAX_LRU) {
                    ++block_lru[i];
                }
            }
        }
    }
    // If any one fail, then the return of this function
    // is fail, otherwise success. We clear the dirty bit
    // on all buffers since the data was written.
    // If the block failed to write, its dirty bit is still set
    // after this call to true.
    return rtvl;
}

block_rtvl_t block_disk_write(int dev, block_t block, char *data)
{
    register block_t i = 0,
             found = BLOCK_NOPOS;

    if(curr >= BLOCK_ARRAY_SIZE) {
        curr = 0;
    }
    for(i = curr; i < BLOCK_ARRAY_SIZE; i++) {
        if(block_map[i] == block && block_dev[i] == dev) {
            found = i;
            break;
        }
    }
    if(curr && found == BLOCK_NOPOS) {
        for(i = 0; i < curr; i++) {
            if(block_map[i] == block && block_dev[i] == dev) {
                found = i;
                break;
            }
        }
    }
    if(found != BLOCK_NOPOS) {
        if(memcmp(block_array[i], data, DEV_BLOCK_SIZE) != 0) {
            memcpy(block_array[i], data, DEV_BLOCK_SIZE); 
        }
        if(dev_scan(dev,block) != DEV_OK) {
            return BLOCK_FAIL; 
        }
        if(dev_write(dev, block, block_array[i]) != DEV_OK) {
            return BLOCK_FAIL;
        }
        return BLOCK_OK;
    }
    // It wasn't in the cache, write the user supplied buffer.
    // The next time it is read, it will not be in the cache,
    // and the 'block_read' will employ the buffer replacement algorithm
    // to find the next slot and will do a disk read to fill the
    // place in the cache and return the user the appropriate data.
    // If the user doesn't follow up with a 'block_read' they will
    // be out of sync with the cache.
    if(dev_scan(dev,block) != DEV_OK) {
        return BLOCK_FAIL; 
    }
    if(dev_write(dev, block, data) != DEV_OK) {
        return BLOCK_FAIL;
    }
    return BLOCK_OK;
}

block_rtvl_t block_disk_write_sync_cache(int dev, block_t block, char *data)
{
    register block_t i = 0,
             found = BLOCK_NOPOS;

    if(curr >= BLOCK_ARRAY_SIZE) {
        curr = 0;
    }
    for(i = curr; i < BLOCK_ARRAY_SIZE; i++) {
        if(block_map[i] == block && block_dev[i] == dev) {
            found = i;
            break;
        }
    }
    if(curr && found == BLOCK_NOPOS) {
        for(i = 0; i < curr; i++) {
            if(block_map[i] == block && block_dev[i] == dev) {
                found = i;
                break;
            }
        }
    }
    if(found != BLOCK_NOPOS) {
        if(memcmp(block_array[i], data, DEV_BLOCK_SIZE) != 0) {
            memcpy(block_array[i], data, DEV_BLOCK_SIZE); 
        }
        if(dev_scan(dev,block) != DEV_OK) {
            return BLOCK_FAIL; 
        }
        if(dev_write(dev, block, block_array[i]) != DEV_OK) {
            return BLOCK_FAIL;
        }
        return BLOCK_OK;
    }
    // It wasn't in the cache, write the user supplied buffer.
    // The next time it is read, it will not be in the cache,
    // and the 'block_read' will employ the buffer replacement algorithm
    // to find the next slot and will do a disk read to fill the
    // place in the cache and return the user the appropriate data.
    // This follows up with a 'block_read' to keep the cache in sync.
    if(dev_scan(dev,block) != DEV_OK) {
        return BLOCK_FAIL; 
    }
    if(dev_write(dev, block, data) != DEV_OK) {
        return BLOCK_FAIL;
    }
    return block_read(dev, block, data);
}

block_rtvl_t block_close(int dev)
{
    bool error = false;
    if(block_sync(dev) != BLOCK_OK) {
        error = true; 
    }
    if(dev_close(dev) != DEV_OK) {
        error = true;
    }
    if(error) {
        return BLOCK_FAIL;
    }
}

void block_reinit()
{
    // See inode.c, when initializing the fs, we call block_open 
    // twice, once to see if we have a fs, and again after inode_mkfs,
    // the second time we need to reinitialize our cache otherwise
    // we will read from a stale cache.
    init = false;
}

#ifdef _TEST_BLOCK
int main(int argc, char **argv)
{
   // This test checks the buffer logic in reading/writing the first three blocks
   // then writes 4096*3 blocks (purposefully greater than the number of
   // blocks in the cache).
   // It was verified that the cache successfully writes to the disk
   // all of the blocks and reads them ok.
   block_t i = 0, dev = 0;
   char buf[DEV_BLOCK_SIZE]={0};
   // TODO - Finish writing test driver.
   if(block_open("./block_cache.dat", &dev) != BLOCK_OK) {
        printf("error opening cache\n");
        return 1;
   }
   if(block_read(dev, 0, buf) != BLOCK_OK) {
        printf("error reading block\n");
        return 1;
   }
   strcpy(buf, "hello world\n");
   if(block_write(dev, 0, buf) != BLOCK_OK) {
        printf("error writing block\n");
        return 1;
   }
   if(block_read(dev, 1, buf) != BLOCK_OK) {
        printf("error reading block\n");
        return 1;
   }
   strcpy(buf, "hello world(2)\n");
   if(block_write(dev, 1, buf) != BLOCK_OK) {
        printf("error writing block\n");
        return 1;
   }
   if(block_read(dev, 2, buf) != BLOCK_OK) {
        printf("error reading block\n");
        return 1;
   }
   strcpy(buf, "hello world(3)\n");
   if(block_write(dev, 2, buf) != BLOCK_OK) {
        printf("error writing block\n");
        return 1;
   }
   for(i = 3; i < (4096 * 3); ++i) {
        if(block_read(dev, i, buf) != BLOCK_OK) {
            printf("error reading block\n");
            return 1;
        }
        sprintf(buf,"%d\n",i);
        if(block_write(dev, i, buf) != BLOCK_OK) {
            printf("error writing block\n");
            return 1;
        }
   }
   printf("write ok\n");
   for(i = 3; i < (4096 * 3); ++i) {
        if(block_read(dev, i, buf) != BLOCK_OK) {
            printf("error reading block\n");
            return 1;
        }
        if(strtoul(buf, NULL, 10) != i) {
            printf("error reading block\n");
        }
   }
   printf("read ok\n");
   if(block_close(dev) != BLOCK_OK) {
        printf("error closing cache\n");
        return 1;
   }
   return 0;
}
#endif
