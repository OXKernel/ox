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
//      file.c
// 
// @description:
//      Implementation of file specific functionality.
//      This is based on our inode implementation.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#include "bool.h"
#include "paths.h"
#include "block.h"
#include "dev.h"
#include "inode.h"
#include "compat.h"

#ifdef _TEST_FILE_INC
#include <stdio.h>
#include <string.h>
#define printk printf
#else
#include <ox/error_rpt.h>
// TODO - Include the rest of the headers.
#endif

#include "file.h"
#include "dir.h"

// file_add_blocks:
//
// Given a device and block_map starting block and
// number of blocks to add, add those blocks in that map.
//
file_rtvl_t file_add_blocks(int dev, 
                            block_t *next, 
                            block_t *current,
                            int     *iblock,
                            block_t *bmap_block, 
                            int nr_blocks, 
                            char *data, 
                            inode_ptr_t length)
{
   block_t i = 0, block = 0;
   block_map_t bmap={0}, tmp={0};
   char zero_data[DEV_BLOCK_SIZE]={0}, dptr[DEV_BLOCK_SIZE]={0};
   inode_ptr_t byte = 0, j = 0;

   if(!bmap_block) {
        errno = EINVAL;
        printk("file_add_blocks:: invalid block (NULL)\n");
        return FILE_FAIL;
   }

   if(*bmap_block == INODE_NULL) {
       // Initialize.
       if(inode_get_data_block(dev, &block) != INODE_OK) {
            errno = ENOSPC;
            printk("file_add_blocks:: unable to allocate data block dev=%d\n",
                    dev);
            return FILE_FAIL;
       }
       // Initial state where inode->next is INODE_NULL.
       *bmap_block = block;
       *next = block;
       *current = block;
       *iblock = 0;
       if(block_write(dev, block, (char *)zero_data) != BLOCK_OK) {
            errno = EACCES;
            printk("file_add_blocks:: unable to write block dev=%d block=%u\n",
                    dev, block);
            return FILE_FAIL;
       }
       if(block_read(dev, block, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_add_blocks:: unable to read block dev=%d block=%u\n",
                    dev, block);
            return FILE_FAIL;
       }
       bmap.next = INODE_NULL;
   } else {
       if(block_read(dev, *bmap_block, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_add_blocks:: unable to read block dev=%d block=%u\n",
                    dev, *bmap_block);
            return FILE_FAIL;
       }
   }
   i = 0;
   while(true) {
       // Scan for empty slot at the end of the file.
       if(bmap.blocks[i] == INODE_NULL && bmap.next == INODE_NULL) {
           *iblock = i;
           break;
       }
       ++i;
       i %= BMAP_BLOCKS;
       if(i == 0) {
           if(bmap.next == INODE_NULL) {
               // There were no empty slots, add another block.
               if(inode_get_data_block(dev, &block) != INODE_OK) {
                    errno = ENOSPC;
                    printk("file_add_blocks:: unable to allocate block dev=%d\n");
                    return FILE_FAIL;
               }
               bmap.next = block;
               if(block_write(dev, *bmap_block, (char *)&bmap) != BLOCK_OK) {
                    errno = EACCES;
                    printk("file_add_blocks:: unable to write block dev=%d block=%u\n",
                            dev, *bmap_block);
                    return FILE_FAIL;
               }
               // current here is current_parent
               // the block prior to the current one we are
               // accessing.
               *current = *bmap_block;
               *bmap_block = block;
               if(block_write(dev, bmap.next, (char *)zero_data) != BLOCK_OK) {
                    errno = EACCES;
                    printk("file_add_blocks:: unable to write block dev=%d block=%u\n",
                            dev, bmap.next);
                    return FILE_FAIL;
               }
               if(block_read(dev, bmap.next, (char *)&tmp) != BLOCK_OK) {
                    errno = EACCES;
                    printk("file_add_blocks:: unable to read block dev=%d block=%u\n",
                            dev, bmap.next);
                    return FILE_FAIL;
               }
               bmap = tmp;
               bmap.next = INODE_NULL;
           } else {
               if(block_read(dev, bmap.next, (char *)&tmp) != BLOCK_OK) {
                    errno = EACCES;
                    printk("file_add_blocks:: unable to read block dev=%d block=%u\n",
                            dev, bmap.next);
                    return FILE_FAIL;
               }
               *current = *bmap_block;
               *bmap_block = bmap.next;
               bmap = tmp;
           }
       }
   }
   // bmap, *bmap_block, and i should be setup from the above loop.
   // We want to append only at the end.
   while(nr_blocks--) {
       // Start adding blocks starting from empty slot.
       *iblock = i;
       if(inode_get_data_block(dev, &block) != INODE_OK) {
            errno = ENOSPC;
            printk("file_add_blocks:: error allocating block dev=%d\n",
                    dev);
            return FILE_FAIL;
       }
       if(bmap.blocks[i] != INODE_NULL) {
            // This function is designed to append the blocks
            // at the end, see its use below. If this is not 
            // the case, we would have to loop back to the beginning
            // (first while loop here, and try to find other empty 
            // slots but that I believe would logically make no sense).
            errno = EACCES;
            printk("file_add_blocks:: inconsistent add block=%u i=%d\n",
                    *bmap_block, i);
            return FILE_FAIL;
       }
       bmap.blocks[i] = block;
       if(block_write(dev, *bmap_block, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_add_blocks:: unable to write block=%u\n",
                    *bmap_block);
            return FILE_FAIL;
       }
       // Do the write if we are writing.
       memset(dptr, 0x0, DEV_BLOCK_SIZE);
       j = 0;
       // Write out the user data, zero pad if we 
       // are at the end and do not have enough to fill
       // a block. 0 out if we are reading.
       while(data && byte < length && j < DEV_BLOCK_SIZE) {
            dptr[j] = data[byte];
            byte++;
            j++;
       }
       if(block_write(dev, block, (char *)dptr) != BLOCK_OK) {
            errno = EACCES;
            printk("file_add_blocks:: error writing block dev=%d block=%u\n",
                    dev, block);
            return FILE_FAIL;
       }
       ++i; 
       i %= BMAP_BLOCKS;
       if (i == 0) {
           // Write the full block map.
           if(bmap.next != INODE_NULL) {
                errno = EACCES;
                printk("file_add_blocks:: inconsistent add block=%u i=%d\n",
                        *bmap_block, i);
                return FILE_FAIL;
           }
           // Get the new block, install it, update the current block,
           // then get the new block.
           if(inode_get_data_block(dev, &block) != INODE_OK) {
                errno = ENOSPC;
                printk("file_add_blocks:: unable to get data block dev=%d\n", dev);
                return FILE_FAIL;
           }
           bmap.next = block;
           if(block_write(dev, *bmap_block, (char *)&bmap) != BLOCK_OK) {
                errno = EACCES;
                printk("file_add_blocks:: unable to write block dev=%d block=%u\n",
                        dev, *bmap_block);
                return FILE_FAIL;
           }
           // current here is the current_parent.
           *current = *bmap_block;
           *bmap_block = block;
           if(block_write(dev, bmap.next, (char *)zero_data) != BLOCK_OK) {
                errno = EACCES;
                printk("file_add_blocks:: unable to write block dev=%d block=%u\n",
                        dev, bmap.next);
                return FILE_FAIL;
           }
           if(block_read(dev, bmap.next, (char *)&tmp) != BLOCK_OK) {
                errno = EACCES;
                printk("file_add_blocks:: unable to read block dev=%d block=%u\n",
                        dev, bmap.next);
                return FILE_FAIL;
           }
           bmap = tmp; 
           bmap.next = INODE_NULL;
       }
   }
   // We need to report the start of the run in terms of the block and index i?
   // The start of the run is reported if the inode->next was null as
   // this is a special case where the list started as empty (see assignment
   // to next above).
   // We assign next, iblock, current_parent, and current
   // see the caller of this function.
   return FILE_OK;
}

//
// file_add_one_bmap_block:
//
// Given a device, current block, and block_map_t,
// add one block_map to it at bmap->next.
//
file_rtvl_t file_add_one_bmap_block(int dev,
                               block_t current,
                               block_map_t *bmap)
{
    // Obtain the actual block_map_t and zero it out.
    block_t block = 0;
    block_map_t zero_data = {0};
    if(inode_get_data_block(dev, &block) != INODE_OK) {
        errno = ENOSPC;
        printk("file_add_one_bmap_block:: failed to get data block dev=%d\n",
                dev);
        return FILE_FAIL;
    }
    bmap->next = block;
    // Must zero out the data.
    if(block_write(dev, block, (char *)&zero_data) != BLOCK_OK) {
        errno = EACCES;
        printk("file_add_one_bmap_block:: failed to write block dev=%d block=%u\n",
                dev, block);
        return FILE_FAIL;
    }
    if(block_write(dev, current, (char *)bmap) != BLOCK_OK) {
        errno = EACCES;
        printk("file_add_one_bmap_block:: failed to write block dev=%d block=%u\n",
                dev, current);
        return FILE_FAIL;
    }
    return FILE_OK;
}

//
// file_add_one_data_block:
//
// Given a device, current block, and block_map_t,
// add a data block at iblock location.
//
file_rtvl_t file_add_one_data_block(int dev, 
                               block_t current, 
                               block_map_t *bmap, 
                               block_t iblock)
{
    // Obtain a data block, and install it into
    // the block_map_t at current.
    block_t block = 0;
    char zero_data[DEV_BLOCK_SIZE]={0};
    if(inode_get_data_block(dev, &block) != INODE_OK) {
        errno = ENOSPC;
        printk("file_add_one_data_block:: failed to get data block dev=%d\n", dev);
        return FILE_FAIL;
    }
    bmap->blocks[iblock] = block;
    if(block_write(dev, block, (char *)zero_data) != BLOCK_OK) {
        errno = EACCES;
        printk("file_add_one_bmap_block:: failed to write block dev=%d block=%u\n",
                dev, block);
        return FILE_FAIL;
    }
    if(block_write(dev, current, (char *)bmap) != BLOCK_OK) {
        errno = EACCES;
        printk("file_add_one_bmap_block:: failed to write block dev=%d block=%u\n",
                dev, current);
        return FILE_FAIL;
    }
    return FILE_OK;
}

file_rtvl_t file_read_write(inode_t *inode, 
                     inode_ptr_t pos, 
                     inode_ptr_t length, 
                     char *data, 
                     bool reading, 
                     inode_ptr_t *bytes_processed)
{
    block_t block = ((block_t)(pos / DEV_BLOCK_SIZE) + 
                        ((pos % DEV_BLOCK_SIZE)?1:0) +
                        ((pos == 0)?1:0));
    block_t byte  = (block_t)pos % DEV_BLOCK_SIZE; // Where in the block to start.
    block_t iter  = ((block_t)block / BMAP_BLOCKS + 
                        ((block % BMAP_BLOCKS)?1:0)); // Number of scans to make to start.
    block_t iblock= (block_t)(pos / DEV_BLOCK_SIZE) %
                        BMAP_BLOCKS; // Which starting block, starting at 0.
    block_t dblock= ((block_t)length / DEV_BLOCK_SIZE + ((length % DEV_BLOCK_SIZE)?1:0));
    block_t dbyte = (block_t)length % DEV_BLOCK_SIZE;
    block_t i = 0, start = 0, current = 0;
    char dptr[DEV_BLOCK_SIZE]={0};
    block_t block_start = inode->next;
    int dev = inode->dev;
    block_map_t bmap={0}, tmp={0};
    bool write_flag = false;

    if(!length) {
        // No work specified.
        *bytes_processed = 0;
        return FILE_OK;
    }

    // We need the correct uninitialized state
    // where pos == 0 and inode->next == INODE_NULL,
    // we therefore get a bmap block and a data block, and set start to
    // it and iblock to 0. And load bmap correctly.
    // Also, another case is we are initialized but the file has come under
    // first use, in which case the inode->current is 0, and therefore
    // we can not load the initialization code.
    if(inode->next == INODE_NULL) {
        char zero_data[DEV_BLOCK_SIZE]={0};
        if(inode_get_data_block(dev, &block) != INODE_OK) {
            printk("file_read_write:: failed to get data block dev=%d\n", dev);
            return FILE_FAIL;
        }
        inode->next = block;
        if(block_write(dev, inode->self, (char *)inode) != BLOCK_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to write block dev=%d block=%u\n", 
                    dev, inode->self);
            return FILE_FAIL;
        }
        if(block_write(dev, inode->next, (char *)zero_data) != BLOCK_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to write block dev=%d block=%u\n", 
                    dev, inode->next);
            return FILE_FAIL;
        }
        if(block_read(dev, inode->next, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to read block dev=%d block=%u\n", 
                    dev, inode->next);
            return FILE_FAIL;
        }
        if(inode_get_data_block(dev, &block) != INODE_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to get block dev=%d\n", 
                    dev);
            return FILE_FAIL;
        }
        bmap.blocks[0] = block;
        if(block_write(dev, inode->next, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to write block dev=%d block=%u\n", 
                    dev, inode->next);
            return FILE_FAIL;
        }
        start = block;
        iblock = 0;
        current = inode->next;
    } else if(inode->pos == 0 && inode->current == INODE_NULL) {
        current = inode->next;
        if(block_read(dev, current, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to read block dev=%d block=%u\n", 
                    dev, current);
            return FILE_FAIL;
        }
        start = bmap.blocks[0];
        iblock = 0;
    } else if(inode->pos == pos && 
              inode->current != INODE_NULL &&
              inode->iblock != INODE_NOPOS) {
        // Setup the read starting from what was stored in the inode
        // from previous.
        start = inode->current;
        iblock = inode->iblock;
        if(block_read(dev, inode->current_parent, (char *)&bmap) != BLOCK_OK) {
            errno = EACCES;
            printk("file_read_write:: failed to read block dev=%d block=%u\n", 
                    dev, inode->current_parent);
            return FILE_FAIL;
        }
        current = inode->current_parent;
    } else {
        // Scan to the desired position.
        // NOTE: iter and iblock in this case were
        // precomputed at the beginning of this function.
        // DEBUG 
        printk("iter=%d\n",iter);
        for(i = 0, bmap.next = inode->next; i < iter; ++i) {
            current = bmap.next; // The current block.
            if(block_read(dev, bmap.next, (char *)&tmp) != BLOCK_OK) { // Read the next pointer and assign to tmp.
                errno = EACCES;
                printk("file_read_write:: error reading block dev=%d block=%u\n",
                        dev, inode->next);
                return FILE_FAIL;
            }
            bmap = tmp; // Advance in the list.
        }
        // find starting block.
        start = bmap.blocks[iblock];
    }

    // In all cases above, we need start, current, and iblock
    // to be set up prior to the read/write.
    if(block_read(dev, start, (char *)&dptr) != BLOCK_OK) {
        errno = EACCES;
        printk("file_read_write:: error reading block dev=%d block=%u\n",
                dev, inode->next);
        return FILE_FAIL;
    }

    // We can derive a write from this by swapping dptr[byte] = data[i]
    // and when byte == 0, we block_write(dev, start, dptr), before doing any
    // reading.
    i = 0;
    *bytes_processed = 0;
    do {
        if(reading) {
            if(*bytes_processed >= inode->size) {
                data[i] = 0;
                dptr[byte] = 0;
            } else {
                data[i] = dptr[byte];
            }
        } else {
            write_flag = true;
            dptr[byte] = data[i];
        }
        i++;
        *bytes_processed = i;
        ++byte; 
        byte %= DEV_BLOCK_SIZE;
        length--;
        if(byte == 0) {
            if(!reading || *bytes_processed >= inode->size) {
                write_flag = false;
                if(block_write(dev, start, (char *)&dptr) != BLOCK_OK) {
                    errno = EACCES;
                    printk("file_read_write:: error writing block dev=%d block=%u\n",
                            dev, start);
                    return FILE_FAIL;
                }
            }
            // Read next block.
            ++iblock; 
            iblock %= BMAP_BLOCKS;
            if(iblock == 0) {
                if(bmap.next == INODE_NULL) {
                    // Scan for a free data block, once found
                    // clear data, add it to bmap.next and write
                    // bmap (which is referenced on disk as current).
                    // The start == INODE_NULL code will
                    // fill it.
                    if(file_add_one_bmap_block(dev, current, &bmap) != FILE_OK) {
                        printk("file_read_write:: error adding block to dev=%d block=%u\n",
                                dev, current);
                        return FILE_FAIL;
                    }
                }
                // If bmap.next was INODE_NULL, then we added a new block_map to it
                // and it should be referenced by bmap.next, otherwise,
                // we came here and bmap.next was not INODE_NULL in the first place.
                current = bmap.next; // This is the current block, bmap.next will point to next after load.
                if(block_read(dev, bmap.next, (char *)&tmp) != BLOCK_OK) {
                    errno = EACCES;
                    printk("file_read_write:: error reading block dev=%d block=%u\n",
                            dev, bmap.next);
                    return FILE_FAIL;
                }
                bmap = tmp;
            }
            start = bmap.blocks[iblock];
            if(start == INODE_NULL) {
                // Scan for a free data block, once found
                // clear data, add it to bmap.block[iblock] and write
                // bmap (which is referenced on disk as current).
                if(file_add_one_data_block(dev, current, &bmap, iblock) != FILE_OK) {
                    printk("file_read_write:: error adding one data block dev%d block=%u iblock=%d\n",
                            dev, block, iblock);
                    return FILE_FAIL;
                }
            }
            start = bmap.blocks[iblock];
            if(block_read(dev, start, (char *)&dptr) != BLOCK_OK) {
                errno = EACCES;
                printk("file_read_write:: error reading block dev=%d start=%u\n",
                        dev, start);
                return FILE_FAIL;
            }
        }
    } while(length);
    if(write_flag) {
       // This occurs when length < DEV_BLOCK_SIZE as the above
       // loop will only write when we cross a boundary.
       // It also occurs at the last block write if its less
       // than DEV_BLOCK_SIZE.
       if(block_write(dev, start, (char *)&dptr) != BLOCK_OK) {
                errno = EACCES;
                printk("file_read_write:: error writing block dev=%d block=%u\n",
                       dev, start);
                return FILE_FAIL;
       }
    }
    // Setup our locations for the next run.
    // We must setup the pos, size, current, current_parent, and iblock.
    // Originally we used length, but this is decremented to 0 
    // in the read/write loop above.
    inode->pos  = pos + *bytes_processed;
    if(inode->size < pos + *bytes_processed) {
        // The reasoning here is that we have pos + length where
        // if pos was >= size we handled in a special case above,
        // otherwise, pos < size and the new size is pos + length > size.
        // Another way to look at is we have:
        // ((pos + length) - inode->size) + inode->size
        // where the inode->size terms cancel out.
        inode->size = pos + *bytes_processed;
    }
    inode->current = start;
    inode->current_parent = current; 
    inode->iblock = iblock;
    if(block_write(dev, inode->self, (char *)inode) != BLOCK_OK) {
        errno = EACCES;
        printk("file_read_write:: error writing block dev=%d block=%u\n",
                dev, inode->self);
        return FILE_FAIL;
    }
    return FILE_OK;
}

//
// RGDTODO := Double check this code. Especially open, open2, creat, close, etc.
// We need a way to determine what dev we are on, re-read the block.c and inode.c
// code to figure this out. We may want to hard code it for now and provide an api
// to get it later.
//
//  This allows us to implement:
//  creat, open, close, read, write, lseek, dup, dup2,
//  unlink, chmod, fchmod, stat, fstat, lstat, sync,
//  getcwd, getwd, get_current_dir_name, chdir, fchdir.
//  There are 20 system calls, with dir.c and link.c, we may end
//  up with about 30-35. With proc subsystem, we may end up
//  with another 10-30 calls. Stop there and release.
//
//  NOTE: tell is 'lseek(fd, 0, SEEK_CUR)'
//

int kopen(const char *path, int flags)
{
    inode_t inode;
    int i = 0, fd = 0;
    bool found = false;
    int dev = master_get_dev(path);
    inode_mode_t imode = 0;
    inode_perm_t  perm = 0;
    block_map_t bmap = {0};
    block_t current = 0;

    if(flags & O_RDONLY) {
        imode = INODE_READ;
        perm |= S_IRUSR;
    }
    if(flags & O_WRONLY) {
        imode = INODE_WRITE;
        perm |= S_IWUSR;
    }
    if(flags & O_RDWR || flags & O_TRUNC) {
        imode = INODE_RW;
        perm |= S_IRUSR;
        perm |= S_IWUSR;
    }
    if(inode_get(dev, current_process->cwd, path, true, imode, &inode) != INODE_OK) {
        if(flags & O_CREAT) {
            if(inode_create(dev, 
                         current_process->cwd, 
                         path, 
                         INODE_CREATE_FILE, 
                         perm,
                         current_process->umask, 
                         current_process->group,
                         current_process->owner,
                         NULL) != INODE_OK) {
                errno = EACCES;
                printk("open:: error creating file\n");
                return -1;
            }
            if(inode_get(dev, current_process->cwd, path, true, 
                         imode, &inode) != INODE_OK) {
                errno = EACCES;
                printk("open:: error accessing inode [%s]\n", path);
                return -1;
            }
        } else {
           /* Error opening file. */
           errno = EACCES;
           printk("open:: error opening file\n");
           return -1;
        }
    }
    inode.accessed_time = ktime(0);
    // Truncate if needed. Need to write code to do this.
    // Basically, copy the inode, then call a truncate method based
    // on inode_free. Basically a routine to free the data blocks 
    // pointed to by the inode.
    if(flags & O_TRUNC) {
        inode.modified_time = ktime(0);
        bmap.next = inode.next;
        current = bmap.next;
        while(current != INODE_NULL) {
            current = bmap.next;
            block_read(dev, bmap.next, (char *)&bmap);
            for(i = 0; i < BMAP_BLOCKS; ++i) {
                if(bmap.blocks[i] != INODE_NULL) {
                    // DEBUG printk("bmap.blocks[%d]=%u\n",i,bmap.blocks[i]);
                    inode_free_data_block(dev, bmap.blocks[i]);
                }
            }
            if(current != INODE_NULL) {
                inode_free_data_block(dev, current);
            }
        }
        inode.pos  = 0;
        inode.size = 0;
        inode.next = INODE_NULL;
        block_write(dev, inode.self, (char *)&inode);
    }
    if(flags & O_RDONLY) {
        inode.o_mode |= O_RDONLY;
    } else if(flags & O_WRONLY) {
        inode.o_mode |= O_WRONLY;
    } else if(flags & O_RDWR) {
        inode.o_mode |= O_RDWR;
    }
    if(flags & O_APPEND) {
        inode.current = INODE_NULL;
        inode.pos = inode.size;
        inode.current_parent = INODE_NULL;
        inode.iblock = 0;
        inode.dev = dev;
    } else {
        inode.current = INODE_NULL;
        inode.pos = 0;
        inode.current_parent = INODE_NULL;
        inode.iblock = 0;
        inode.dev = dev;
    }
  // Determine if a file has been allocated.
  for(i = 0, fd = -1; i < MAX_FILES; ++i) {
      if(!current_process->file_tab[i].path || !strlen(current_process->file_tab[i].path)) {
          for(fd = 0; fd < MAX_FILES; ++fd) {
              if(!current_process->file_desc[fd]) {
                  current_process->file_desc[fd] = &current_process->file_tab[i];
                  found = true;
                  break;
              }
          }
          if(!found) {
              fd = -1;
          } else if(found) {
              break;
          }
      }
  }
  // fd points to the file descriptor.
  // Now load it using inode_get or inode_create.
  if(fd == -1) {
      errno = EMFILE;
      printk("open:: error maximum file descriptors in use\n");
      return fd;
  }
  // Struct assign.
  *(current_process->file_desc[fd]) = inode;
  return fd;
}

int kopen2(const char *path, int flags, mode_t mode)
{
    inode_t inode;
    int i = 0, fd = 0;
    bool found = false;
    int dev = master_get_dev(path);
    inode_mode_t imode = 0;
    inode_perm_t  perm = 0;
    block_map_t bmap = {0};
    block_t current = 0;

    if(flags & O_RDONLY) {
        imode = INODE_READ;
        perm |= S_IRUSR;
    }
    if(flags & O_WRONLY) {
        imode = INODE_WRITE;
        perm |= S_IWUSR;
    }
    if(flags & O_RDWR || flags & O_TRUNC) {
        imode = INODE_RW;
        perm |= S_IRUSR;
        perm |= S_IWUSR;
    }
    if(inode_get(dev, current_process->cwd, path, true, imode, &inode) != INODE_OK) {
        if(flags & O_CREAT) {
            if(inode_create(dev, 
                         current_process->cwd, 
                         path, 
                         INODE_CREATE_FILE, 
                         mode,
                         current_process->umask, 
                         current_process->group,
                         current_process->owner,
                         NULL) != INODE_OK) {
                errno = EACCES;
                printk("open:: error creating file\n");
                return -1;
            }
            if(inode_get(dev, current_process->cwd, path, true, 
                         imode, &inode) != INODE_OK) {
                errno = EACCES;
                printk("open:: error accessing inode [%s]\n", path);
                return -1;
            }
        } else {
           /* Error opening file. */
           errno = EACCES;
           printk("open:: error opening file\n");
           return -1;
        }
    }
    inode.accessed_time = ktime(0);
    // Truncate if needed. Need to write code to do this.
    // Basically, copy the inode, then call a truncate method based
    // on inode_free. Basically a routine to free the data blocks 
    // pointed to by the inode.
    if(flags & O_TRUNC) {
        inode.modified_time = ktime(0);
        bmap.next = inode.next;
        current = bmap.next;
        while(current != INODE_NULL) {
            current = bmap.next;
            block_read(dev, bmap.next, (char *)&bmap);
            for(i = 0; i < BMAP_BLOCKS; ++i) {
                if(bmap.blocks[i] != INODE_NULL) {
                    inode_free_data_block(dev, bmap.blocks[i]);
                }
            }
            if(current != INODE_NULL) {
                inode_free_data_block(dev, current);
            }
        }
        inode.pos  = 0;
        inode.size = 0;
        inode.next = INODE_NULL;
        block_write(dev, inode.self, (char *)&inode);
    }
    if(flags & O_RDONLY) {
        inode.o_mode |= O_RDONLY;
    } else if(flags & O_WRONLY) {
        inode.o_mode |= O_WRONLY;
    } else if(flags & O_RDWR) {
        inode.o_mode |= O_RDWR;
    }
    if(flags & O_APPEND) {
        inode.current = INODE_NULL;
        inode.pos = inode.size;
        inode.current_parent = INODE_NULL;
        inode.iblock = 0;
        inode.dev = dev;
    } else {
        inode.current = INODE_NULL;
        inode.pos = 0;
        inode.current_parent = INODE_NULL;
        inode.iblock = 0;
        inode.dev = dev;
    }
  // Determine if a file has been allocated.
  for(i = 0, fd = -1; i < MAX_FILES; ++i) {
      if(!current_process->file_tab[i].path || !strlen(current_process->file_tab[i].path)) {
          for(fd = 0; fd < MAX_FILES; ++fd) {
              if(!current_process->file_desc[fd]) {
                  current_process->file_desc[fd] = &current_process->file_tab[i];
                  found = true;
                  break;
              }
          }
          if(!found) {
              fd = -1;
          } else if(found) {
              break;
          }
      }
  }
  // fd points to the file descriptor.
  // Now load it using inode_get or inode_create.
  if(fd == -1) {
      errno = EMFILE;
      printk("open:: error maximum file descriptors in use\n");
      return fd;
  }
  // Struct assign.
  *(current_process->file_desc[fd]) = inode;
  return fd;
}

int kcreat(const char *path, mode_t mode)
{
    inode_t inode = {0};
    int i = 0, fd = 0;
    int dev = master_get_dev(path);
    inode_mode_t imode = 0;
    bool found = false;

    imode = inode_get_mode(mode, current_process->umask);

    if(inode_create(dev, 
                    current_process->cwd, 
                    path, 
                    INODE_CREATE_FILE, 
                    mode,
                    current_process->umask, 
                    current_process->group,
                    current_process->owner,
                    NULL) != INODE_OK) {
       errno = EACCES;
       printk("creat:: error creating file\n");
       return -1;
   }
   // Determine if a file has been allocated.
   for(i = 0, fd = -1; i < MAX_FILES; ++i) {
      if(!current_process->file_tab[i].path || !strlen(current_process->file_tab[i].path)) {
          for(fd = 0; fd < MAX_FILES; ++fd) {
              if(!current_process->file_desc[fd]) {
                  current_process->file_desc[fd] = &current_process->file_tab[i];
                  found = true;
                  break;
              }
          }
          if(!found) {
              fd = -1;
          } else if(found) {
              break;
          }
      }
   }
   // fd points to the file descriptor.
   // Now load it using inode_get.
   if(fd == -1) {
      errno = EMFILE;
      printk("creat:: error maximum file descriptors in use\n");
      return fd;
   }

   if(inode_get(dev, current_process->cwd, path, true, imode, &inode) != INODE_OK) {
        errno = EMFILE;
        printk("creat:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
   }

   // Must initialize the inode for read/write here.
   inode.current = INODE_NULL;
   inode.pos = 0;
   inode.current_parent = INODE_NULL;
   inode.iblock = 0;
   inode.dev = dev;

   // Struct assign.
   *(current_process->file_desc[fd]) = inode;
   return fd;
}

int kunlink(const char *path)
{
    int dev = master_get_dev(path);
    if(inode_free(dev, current_process->cwd, path, NULL) != INODE_OK) {
        printk("unlink:: error unlinking file\n");
        return -1;
    }
    return 0;
}

int kclose(int fd)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    inode_t zero_inode = {0};
    bool found = false;
    int i = 0;

    if(!inode) {
        printk("close:: invalid file desc [%d]\n",fd);
        errno = EBADF;
        return -1;
    }

    for(i = 0; i < MAX_FILES; ++i) {
        if(current_process->file_desc[i] == inode && i != fd) {
            found = true;
            break;
        }
    }

    if(found) {
        // There was a duplicate, just NULL out
        // this descriptor.
        current_process->file_desc[fd] = NULL;
        return 0;
    }

    // Otherwise, continue with close.
    if(block_sync(inode->dev) != BLOCK_OK) {
        errno = EACCES;
        printk("close:: error writing blocks dev=%d\n", inode->dev);
        return -1;
    }
    inode->o_mode  = 0;
    inode->current = INODE_NULL;
    inode->current_parent = INODE_NULL;
    inode->iblock = 0;
    inode->pos = 0;
    inode->accessed_time = ktime(0);
    if(block_write(inode->dev, inode->self, (char *)inode) != BLOCK_OK) {
        errno = EACCES;
        printk("close:: error writing inode dev=%d block=%u\n",
                inode->dev, inode->self);
        return -1;
    }
    // Reset the file table.
    for(i = 0; i < MAX_FILES; ++i) {
        if(&(current_process->file_tab[i]) == inode) {
            current_process->file_tab[i] = zero_inode;
        }
    }
    // NULL out our descriptor table indicating the
    // descriptor is ready for use for some other file.
    current_process->file_desc[fd] = NULL;
    return 0;
}

ssize_t kread(int fd, void *buf, size_t count)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    inode_ptr_t bytes_processed = 0, bytes_processed1 = 0, tmp = 0;
    if(!inode) {
        printk("read:: invalid file desc [%d]\n",fd);
        return -1;
    }
    inode->accessed_time = ktime(0);
    if(inode->pos > inode->size) {
        char zero_data[DEV_BLOCK_SIZE]={0};
        block_t iter = (inode->pos - inode->size) / DEV_BLOCK_SIZE;
        block_t  mod = (inode->pos - inode->size) % DEV_BLOCK_SIZE;
        while(iter--) {
            if(file_read_write(inode, inode->size, DEV_BLOCK_SIZE, 
                           zero_data, true /* reading */, &tmp) != FILE_OK) {
                return -1;
            }
            bytes_processed1 += tmp;
        }
        if(mod) {
            if(file_read_write(inode, inode->size, mod, 
                           zero_data, true /* reading */, &tmp) != FILE_OK) {
                return -1;
            }
            bytes_processed1 += tmp;
        }
    }
    //printk("RGD count=%u\n",count);
    if(file_read_write(inode, inode->pos, count, buf, true /* reading */, &bytes_processed) != FILE_OK) {
        return -1;
    }
    //printk("buf=[%s]\n",buf);
    return bytes_processed + bytes_processed1;
}

ssize_t kwrite(int fd, const void *buf, size_t count)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    inode_ptr_t bytes_processed = 0, bytes_processed1 = 0, tmp = 0;
    if(!inode) {
        printk("write:: invalid file desc [%d]\n",fd);
        return -1;
    }
    inode->modified_time = ktime(0);
    if(inode->pos > inode->size) {
        char zero_data[DEV_BLOCK_SIZE]={0};
        block_t iter = (inode->pos - inode->size) / DEV_BLOCK_SIZE;
        block_t  mod = (inode->pos - inode->size) % DEV_BLOCK_SIZE;
        while(iter--) {
            if(file_read_write(inode, inode->size, DEV_BLOCK_SIZE, 
                           zero_data, false /* writing */, &tmp) != FILE_OK) {
                return -1;
            }
            bytes_processed1 += tmp;
        }
        if(mod) {
            if(file_read_write(inode, inode->size, mod, 
                           zero_data, false /* writing */, &tmp) != FILE_OK) {
                return -1;
            }
            bytes_processed1 += tmp;
        }
    }
    if(file_read_write(inode, inode->pos, count, buf, false /* writing */, &bytes_processed) != FILE_OK) {
        return -1;
    }
    return bytes_processed + bytes_processed1;
}

// RGDTODO - Define off_t, SSIZE_MAX, sssize_t,
//           are they in ox/types.h or sys/types.h ?
off_t klseek(int fd, off_t offset, int whence)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    if(!inode) {
        printk("lseek:: invalid file desc [%d]\n",fd);
        return (off_t)-1;
    }
    if(whence == SEEK_SET) {
        inode->pos = offset;
    } else if(whence == SEEK_CUR) {
        inode->pos += offset;
        if(inode->pos > SSIZE_MAX) {
            // Its actually unspecified, but we hard code
            // it to be the largest value.
            inode->pos = SSIZE_MAX;
        }
    } else if(whence == SEEK_END) {
        inode->pos = inode->size + offset;
    } else {
        return (off_t)-1;
    }
    // Must reset iblock as we need to
    // seek inside file_read_write since
    // an lseek was done.
    inode->iblock = INODE_NOPOS;
    if(block_write(inode->dev, inode->self, (char *)inode) != BLOCK_OK) {
        errno = EACCES;
        printk("lseek:: error writing block dev=%d block=%u\n",
                inode->dev, inode->self);
        return (off_t)-1;
    }
    return inode->pos;
}

//   dup and dup2 are implemented using two tables
//   inside struct proc :=
//
//   inode_t file_tab[MAX_FILES]; and
//   inode_t *file_desc[MAX_FILES];
//
//   These allow us to duplicate the files.
//
int kdup(int fd)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    int i = 0;
    if(!inode) {
        errno = EBADF;
        printk("dup:: error bad file descriptor\n");
        return -1;
    }
    for(i = 0; i < MAX_FILES; ++i) {
        if(current_process->file_desc[i] == NULL) {
            current_process->file_desc[i] = inode;
            return i;
        }
    }
    errno = EMFILE;
    printk("dup:: error too many open files\n");
    return -1;
}

int kdup2(int fd, int newfd)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    if(!inode) {
        errno = EBADF;
        printk("dup2:: error bad file descriptor\n");
        return -1;
    }
    if(newfd < 0 || newfd > MAX_FILES) {
        errno = EBADF;
        printk("dup2:: error bad file descriptor\n");
        return -1;
    }
    if(current_process->file_desc[newfd] == inode) {
        // No-op.
        return newfd;
    }
    if(current_process->file_desc[newfd]) {
        // Close this first.
        kclose(newfd);
    }
    // Duplicate it.
    current_process->file_desc[newfd] = inode;
    return newfd;
}

// RGDTODO - We can also implement these system calls as well.
//         - Do this last. Requires getting/creating a mock struct stat.
//           Load the inode, then fill in the stat buffer fields from the inode.
int kstat(const char *path, struct stat *buf)
{
    inode_t inode;
    int dev = master_get_dev(path);
    if(!buf) {
        errno = EINVAL;
        printk("stat:: NULL buffer\n");
        return -1;
    }
    if(inode_get(dev, current_process->cwd,path,true,INODE_DONTCHECKPERMS,&inode) != INODE_OK) {
        errno = EMFILE;
        printk("stat:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
    }
    // Fill the buffer with data from the inode.
    // inode.accessed_time = ktime(0);
    buf->st_dev  = dev;
    buf->st_ino  = inode.self;
    buf->st_mode = inode_get_permissions(&inode);
    buf->st_nlink= inode.refcount;
    buf->st_uid  = inode.owner;
    buf->st_gid  = inode.group;
    buf->st_rdev = 0; // RGDTODO - Implement device ID.
    buf->st_size = inode.size;
    buf->st_blksize= DEV_BLOCK_SIZE;
    buf->st_blocks = inode.size / DEV_BLOCK_SIZE + ((inode.size % DEV_BLOCK_SIZE)?1:0);
    buf->st_atime = inode.accessed_time; 
    buf->st_mtime = inode.modified_time;
    buf->st_ctime = inode.create_time;
    return 0;
}

int kfstat(int fd, struct stat *buf)
{
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    int dev = master_get_dev(0);
    if(!inode) {
        errno = EBADF;
        printk("fstat:: invalid file desc [%d]\n",fd);
        return -1;
    }
    if(!buf) {
        errno = EINVAL;
        printk("stat:: buffer is NULL\n");
        return -1;
    }
    // Fill the buffer with data from the inode.
    // inode->accessed_time = ktime(0);
    buf->st_dev  = dev;
    buf->st_ino  = inode->self;
    buf->st_mode = inode_get_permissions(inode);
    buf->st_nlink= inode->refcount;
    buf->st_uid  = inode->owner;
    buf->st_gid  = inode->group;
    buf->st_rdev = 0; // RGDTODO - Implement device ID.
    buf->st_size = inode->size;
    buf->st_blksize= DEV_BLOCK_SIZE;
    buf->st_blocks = inode->size / DEV_BLOCK_SIZE + ((inode->size % DEV_BLOCK_SIZE)?1:0);
    buf->st_atime = inode->accessed_time; 
    buf->st_mtime = inode->modified_time;
    buf->st_ctime = inode->create_time;
    return 0;
}

int klstat(const char *path, struct stat *buf)
{
    inode_t inode;
    int dev = master_get_dev(path);
    if(!buf) {
        errno = EINVAL;
        printk("stat:: NULL buffer\n");
        return -1;
    }
    // Stat the link itself.
    if(inode_get(dev, 
                 current_process->cwd, 
                 path, 
                 false /* readlink */, 
                 INODE_DONTCHECKPERMS,
                 &inode) != INODE_OK) {
        errno = EMFILE;
        printk("stat:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
    }
    // Fill the buffer with data from the inode.
    // inode.accessed_time = ktime(0);
    buf->st_dev  = dev;
    buf->st_ino  = inode.self;
    buf->st_mode = inode_get_permissions(&inode);
    buf->st_nlink= inode.refcount;
    buf->st_uid  = inode.owner;
    buf->st_gid  = inode.group;
    buf->st_rdev = 0; // RGDTODO - Implement device ID.
    buf->st_size = inode.size;
    buf->st_blksize= DEV_BLOCK_SIZE;
    buf->st_blocks = inode.size / DEV_BLOCK_SIZE + ((inode.size % DEV_BLOCK_SIZE)?1:0);
    buf->st_atime = inode.accessed_time; 
    buf->st_mtime = inode.modified_time;
    buf->st_ctime = inode.create_time;
    return 0;
}

void ksync(void)
{
    int dev = master_get_dev(0);
    if(block_sync(dev) != BLOCK_OK) {
        errno = EACCES;
        printk("sync:: error sync'ing blocks\n");
    }
}

int kchmod(const char *path, mode_t mode)
{
    // Get inode from path, set the mode bits inside,
    // write out to disk.
    inode_t inode;
    int dev = master_get_dev(0);
    if(inode_get(dev,current_process->cwd,path,true,INODE_DONTCHECKPERMS,&inode) != INODE_OK) {
        errno = EMFILE;
        printk("chmod:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
    }
    if(inode_set_permissions(&inode, mode, current_process->umask) != INODE_OK) {
        errno = EACCES;
        printk("chmod:: error setting permissions block=%u mode=%u mask=%u\n",
                inode.self, mode, current_process->umask);
        return -1;
    }
    inode.modified_time = ktime(0);
    if(block_write(dev, inode.self, (char *)&inode) != BLOCK_OK) {
        errno = EACCES;
        printk("chmod:: error writing block dev=%d block=%u\n",
                dev, inode.self);
        return -1;
    }
    return 0;
}

int kfchmod(int fd, mode_t mode)
{
    // Get inode from file_desc table. Set the mode bits inside,
    // write out to disk.
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    if(!inode) {
        errno = EBADF;
        printk("fchmod:: invalid file desc [%d]\n",fd);
        return -1;
    }
    inode->modified_time = ktime(0);
    if(inode_set_permissions(inode, mode, current_process->umask) != INODE_OK) {
        errno = EACCES;
        printk("chmod:: error setting permissions block=%u mode=%u mask=%u\n",
                inode->self, mode, current_process->umask);
        return -1;
    }
    if(block_write(inode->dev, inode->self, (char *)inode) != BLOCK_OK) {
        errno = EACCES;
        printk("chmod:: error writing block dev=%d block=%u\n",
                inode->dev, inode->self);
        return -1;
    }
    return 0;
}

int kchown(const char *path, uid_t owner, gid_t group)
{
    // Get inode from path, set the ownership,
    // write out to disk.
    inode_t inode;
    int dev = master_get_dev(0);
    if(inode_get(dev,current_process->cwd,path,true,INODE_DONTCHECKPERMS,&inode) != INODE_OK) {
        errno = EMFILE;
        printk("chmod:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
    }
    inode.owner = owner;
    inode.group = group;
    inode.modified_time = ktime(0);
    if(block_write(dev, inode.self, (char *)&inode) != BLOCK_OK) {
        errno = EACCES;
        printk("chmod:: error writing block dev=%d block=%u\n",
                dev, inode.self);
        return -1;
    }
    return 0;
}

int kfchown(int fd, uid_t owner, gid_t group)
{
    // Get inode from file_desc table. Set the mode bits inside,
    // write out to disk.
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    if(!inode) {
        errno = EBADF;
        printk("fchmod:: invalid file desc [%d]\n",fd);
        return -1;
    }
    inode->owner = owner;
    inode->group = group;
    inode->modified_time = ktime(0);
    if(block_write(inode->dev, inode->self, (char *)inode) != BLOCK_OK) {
        errno = EACCES;
        printk("chmod:: error writing block dev=%d block=%u\n",
                inode->dev, inode->self);
        return -1;
    }
    return 0;
}

int klchown(const char *path, uid_t owner, gid_t group)
{
    // Get inode from path, set the ownership,
    // write out to disk.
    inode_t inode;
    int dev = master_get_dev(0);
    if(inode_get(dev,current_process->cwd,path,false /* readlink */,INODE_DONTCHECKPERMS,&inode) != INODE_OK) {
        errno = EMFILE;
        printk("chmod:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
    }
    inode.owner = owner;
    inode.group = group;
    inode.modified_time = ktime(0);
    if(block_write(dev, inode.self, (char *)&inode) != BLOCK_OK) {
        errno = EACCES;
        printk("chmod:: error writing block dev=%d block=%u\n",
                dev, inode.self);
        return -1;
    }
    return 0;
}

char *kgetcwd(char *buf, size_t size)
{
    char *tmp = kget_current_dir_name();
    if(!buf || !size || !tmp ) {
        errno = EFAULT;
        printk("getcwd:: error getting current working dir\n");
        return NULL;
    }
    if(strlen(tmp) >= size) {
        errno = ERANGE;
        printk("getcwd:: error getting working dir "
               "strlen(tmp)=%d >= size=%d tmp=%s\n",
               strlen(tmp), size, tmp);
        return NULL;
    }
    strncpy(buf,tmp,size);
    return buf;
}

char *kgetwd(char *buf)
{
    char *tmp = kget_current_dir_name();
    if(!tmp) {
        errno = EFAULT;
        printk("getwd:: error getting current working dir\n");
        return NULL;
    }
    strcpy(buf, tmp);
    return buf;
}

char *kget_current_dir_name(void)
{
    // Get the inode using
    // the block_t current_process->cwd. 
    // Do a block_read(dev, current_process->cwd, (char *)&inode)
    // Requires a krealpath resolution to get the absolute path of the directory.
    inode_t inode;
    int dev = master_get_dev(0); // RGDTODO - Check this will still work once devices are implemented.
    master_inode_t *master = master_get(dev);
    static char res_path[MAX_PATH]={0};
    memset(res_path,0x0,MAX_PATH);

    if(block_read(dev, current_process->cwd, (char *)&inode) != BLOCK_OK) {
        errno = EACCES;
        printk("get_current_dir_name:: error reading block dev=%d block=%u\n",
                dev, current_process->cwd);
        return NULL;
    }
    // This is being interpreted as always from root "/" directory.
    if(krealpath(dev, INODE_ROOT_BLOCK, inode.path, res_path, &errno) == NULL) {
        errno = EACCES;
        printk("get_current_dir_name:: error resolving path block=%u path=%s\n",
                inode.self, inode.path);
        return NULL;
    }
    // printk("RGD res_path=%s\n",res_path);
    return res_path;
}

int kchdir(const char *path)
{
    // Get the inode.
    // Set the current_process->cwd to inode.self.
    // RGDTODO:= Double check that current working dir works inside inode.c
    inode_t inode = {0};
    int dev = master_get_dev(path);
    if(inode_get(dev, current_process->cwd, path,true,INODE_DONTCHECKPERMS,&inode) != INODE_OK) {
        errno = EMFILE;
        printk("creat:: error retrieving inode dev=%d current_dir=%u path=%s\n",
                dev, current_process->cwd, path);
        return -1;
    }
    if(inode.is_directory) {
        current_process->cwd = inode.self;
        return 0;
    } else {
        return -1;
    }
}

int kfchdir(int fd)
{
    // Lookup the inode referenced by fd,
    // set current_process->cwd to inode.self of that descriptor.
    inode_t *inode = ((fd < 0 || fd > MAX_FILES) ? NULL : current_process->file_desc[fd]);
    if(!inode) {
        errno = EBADF;
        return -1;
    }
    current_process->cwd = inode->self;
    return 0;
}

int kutime(const char *filename, struct utimbuf *buf)
{
    int dev = master_get_dev(filename);
    inode_t inode = {0};
    if(!buf) {
        errno = EINVAL;
        printk("utime:: NULL buffer\n");
        return -1;
    }
    if(inode_get(dev, current_process->cwd, 
                 filename, true, INODE_DONTCHECKPERMS,&inode) != INODE_OK) {
        printk("utime:: error getting inode\n");
        return -1;
    }
    // Update the inode.
    inode.accessed_time = buf->actime;
    inode.modified_time = buf->modtime;
    if(block_write(dev, inode.self, (char *)&inode) != BLOCK_OK) {
        errno = EACCES;
        printk("utime:: error writing block dev=%d block=%u\n",
                dev, inode.self);
        return -1;
    }
    return 0;
}

int krename(const char *oldpath, const char *newpath)
{
    int dev = master_get_dev(oldpath);
    inode_t inode     = {0};
    inode_t newnode   = {0};
    inode_mode_t mode = INODE_CREATE_FILE;

    if(inode_get(dev, current_process->cwd,
                 oldpath, true, INODE_DONTCHECKPERMS, &inode) != INODE_OK) {
        printk("rename:: oldpath does not exist [%s]\n",oldpath);
        return -1;
    }
    if(inode_create(dev, current_process->cwd, newpath,
                    mode, // Create a file, in inode_free we assign the old struct.
                    inode_get_permissions(&inode),
                    current_process->umask,
                    current_process->group,
                    current_process->owner,
                    NULL) != INODE_OK) {
        printk("rename:: error renaming file oldpath=[%s] newpath=[%s]\n",
                oldpath, newpath);
        return -1;
    }
    if(inode_get(dev, current_process->cwd, 
                 newpath, true, INODE_DONTCHECKPERMS, &newnode) != INODE_OK) {
        printk("rename:: newpath does not exist\n");
        return -1;
    }
    if(inode_free(dev, current_process->cwd, oldpath, &newnode) != INODE_OK) {
        printk("rename:: error renaming file oldpath=[%s] newpath=[%s]\n",
                oldpath, newpath);
        return -1;
    }
    if(block_write(dev, newnode.self, (char *)&newnode) != BLOCK_OK) {
        errno = EACCES;
        printk("rename:: error writing block dev=%d block=%u\n",
                dev, newnode.self);
        return -1;
    }
    return 0;
}

#ifdef _TEST_FILE
int
main(int argc, char **argv)
{
    int i  = 0;
    int fd = 0, fd1 = 0;
    int dev = 0;
    master_inode_t *master = NULL;
    char buff[36]={0};
    char buff1[514]={0};
    char buff2[65026 * 2]={0};
    int tmp1 = 0, tmp2 = 0;
    struct stat stbuf={0};
    struct utimbuf timebuf={0};
    char *current = 0;
    char current_buf[MAX_PATH]={0};

    strcpy(buff,"hello world");
    for(i = 0; i < 513; ++i) {
        buff1[i]=i;
        // printk("buff1 i=%d last position=%d\n",i,buff1[i]);
    }

    for(i = 0; i < 65026 * 2; ++i) {
        buff2[i]=i;
        // printk("buff2 i=%d last position=%d\n",i,buff2[i]);
    }

    printk("buff1 pos=511=%d last position=%d\n",buff1[511],buff1[512]);
    printk("buff2 pos=65024 =%d last position=%d\n",buff2[65024],buff2[65025]);
    tmp1 = buff2[65024 * 2];
    tmp2 = buff2[65025 * 2];

    // Obtains the device based on block_open -> dev_open
    // Initialization here is similar to what would happen
    // in the real kernel. We must make the file system first.
    if(inode_dev_open("./inode.dat",&dev) == INODE_INIT) {
       P();
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
    master = master_get(dev);
    master_set_dev("./inode.dat", dev);
    P();
    // Done initializing file system.
    if((fd = kopen("/foo", O_CREAT | O_RDWR)) == -1) {
        printk("file_init:: error opening file\n");
    }
    printk("fd=%d\n",fd);
    if(kwrite(fd, buff, sizeof(buff)) == -1) {
        printk("error writing file\n");
    }
    memset(buff,0x0, sizeof(buff));
    klseek(fd, 1, SEEK_SET);
    if(kread(fd, buff, sizeof(buff)) == -1) {
        printk("error reading file\n");
    }
    if(!strcmp(buff,"ello world")) {
        printk("successfully read buffer\n");
    } else {
        printk("error reading buffer\n");
    }

    klseek(fd, 0, SEEK_SET);
    if(kwrite(fd, buff1, 513) == -1) {
        printk("error writing file\n");
    }
    memset(buff1,0x0, 513);
    klseek(fd, 0, SEEK_SET);
    if(kread(fd, buff1, 513) == -1) {
        printk("error reading file\n");
    }
    if(buff1[511] == -1 && buff1[512] == 0) {
        printk("successfully read buffer\n");
    } else {
        printk("error reading buffer\n");
    }

    // test seek to 512 char (which should be in the next block)
    klseek(fd, 0, SEEK_SET);
    if(kwrite(fd, buff1, 513) == -1) {
        printk("error writing file\n");
    }
    memset(buff1,0x1, 513);
    klseek(fd, 512, SEEK_SET);
    if(kread(fd, buff1, 1) == -1) {
        printk("error reading file\n");
    }
    // Should read the last byte which is 0.
    if(buff1[0] == 0) {
        printk("successfully read buffer\n");
    } else {
        printk("error reading buffer\n");
    }

    // write past the boundary of 127 * 512 bytes
    // contained in a block_map_t structure.
    klseek(fd, 0, SEEK_SET);
    if(kwrite(fd, buff2, 65026 * 2) == -1) {
        printk("error writing file\n");
    }
    memset(buff2,0x0, 65026 * 2);
    klseek(fd, 0, SEEK_SET);
    if(kread(fd, buff2, 65026 * 2) == -1) {
        printk("error reading file\n");
    }
    if(buff2[65024 * 2] == tmp1 && buff2[65025 * 2] == tmp2) {
        printk("successfully read buffer\n");
    } else {
        printk("error reading buffer\n");
    }

    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    if((fd = kopen("/foo", O_CREAT | O_RDWR)) == -1) {
        printk("file_init:: error opening file\n");
    }

    memset(buff2,0x0, 65026 * 2);

    // Test read from previously written file.
    if(kread(fd, buff2, 65026 * 2) == -1) {
        printk("error reading file\n");
    }
    if(buff2[65024 * 2] == tmp1 && buff2[65025 * 2] == tmp2) {
        printk("successfully read buffer\n");
    } else {
        printk("error reading buffer\n");
    }

    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    if((fd = kopen("/foo", O_CREAT | O_RDWR)) == -1) {
        printk("file_init:: error opening file\n");
    }

    memset(buff2,0x0, 65026 * 2);

    // Test double read.
    if(kread(fd, buff2, 2) == -1) {
        printk("error reading file\n");
    }

    if(buff2[1] == 1) {
        printk("successfully read buffer\n");
    }

    if(kread(fd, buff2, 3) == -1) {
        printk("error reading file\n");
    }

    if(buff2[0] == 2) {
        printk("successfully read buffer\n");
    }

    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    // Test O_TRUNC
    if((fd = kopen("/foo", O_CREAT | O_RDWR | O_TRUNC)) == -1) {
        printk("file_init:: error opening file\n");
    }

    if(kstat("/foo", &stbuf) != -1) {
        if(stbuf.st_size != 0) {
            printk("error truncating file\n");
        } else {
            printk("successfully stat'd file and truncated file\n");
        }
    } else {
        printk("error stat'ing file\n");
    }

    memset(buff2,0x1, 65026 * 2);

    if(kread(fd, buff2, 65026 * 2) == -1) {
        printk("error reading file\n");
    }

    for(i = 0; i < 65026 * 2; ++i) {
        if(buff2[i] != 0) {
            printf("error reading buffer i=[%d]\n",i);
            break;
        }
    }

    if(i == 65026 * 2) {
        printf("successfully read buffer\n");
    }

    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    // Test O_APPEND
    // write hello to file, then close, open append mode, write world
    // lseek to 0, re-read, should be hello world.
    if((fd = kopen("/bar", O_CREAT | O_RDWR | O_APPEND)) == -1) {
        printk("file_init:: error opening file\n");
    }
    if(kwrite(fd, "hello ", strlen("hello ")) == -1) {
        printk("error writing file\n");
    }
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }
    if((fd = kopen("/bar", O_CREAT | O_RDWR | O_APPEND)) == -1) {
        printk("file_init:: error opening file\n");
    }
    if(kwrite(fd, "world", strlen("world") + 1) == -1) {
        printk("error writing file\n");
    }
    klseek(fd, 0, SEEK_SET);
    memset(buff2,0x0, 65026 * 2);
    if((i=kread(fd, buff2, strlen("hello world"))) == -1) {
        printk("error reading file\n");
    } else {
        printk("read i=%d bytes\n",i);
    }
    if(memcmp(buff2, "hello world",12)) {
        printk("error reading file, O_APPEND failed\n");
        for(i = 0; i < 14; ++i) {
            if(buff2[i] == 0) { printk("-"); }
            else printk("%c",buff2[i]);
        }
        printk("\n");
    } else {
        printk("successfully read file, O_APPEND worked\n"); 
    }
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }
    if((fd = kopen("/bar", O_CREAT | O_RDWR | O_TRUNC)) == -1) {
        printk("file_init:: error opening file\n");
    }
    klseek(fd, 1025, SEEK_SET);
    if(kwrite(fd, "hello world", strlen("hello world")) == -1) {
        printk("error writing file\n");
    }
    memset(buff2, 0x1, 65026 * 2);
    klseek(fd, 0, SEEK_SET);
    if(kread(fd, buff2, 1025 + strlen("hello world") + 1) == -1) {
        printk("error reading file\n");
    }
    for(i = 0; i < 1025; ++i) {
        if(buff2[i] != 0) {
            break;
        }
    }
    if(i != 1025) {
        printk("error reading file i=[%d]\n",i);
    }
    if(strcmp(buff2 + 1025, "hello world")) {
        printk("error reading file\n");
    } else {
        printk("successfully read file\n");
    }
    klseek(fd, 1025, SEEK_SET);
    if(kread(fd, buff2, strlen("hello world") + 1) == -1) {
        printk("error reading file\n");
    }
    if(strcmp(buff2, "hello world")) {
        printk("error reading file\n");
    } else {
        printk("successfully read file\n");
    }
    fd1 = kdup(fd);
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }
    klseek(fd1, 1025, SEEK_SET);
    if(kread(fd1, buff2, strlen("hello world") + 1) == -1) {
        printk("error reading file\n");
    }
    if(strcmp(buff2, "hello world")) {
        printk("error reading file\n");
    } else {
        printk("successfully read file dup worked\n");
    }
    fd = kdup2(fd1, 17);
    if(fd != 17) {
        printk("error kdup2 not able to dup to specified file descriptor\n");
    } else {
        printk("successfully dup2 of file descriptor\n");
    }
    if((fd1 = kclose(fd1)) == -1) {
        printk("file_init:: error closing file\n");
    }
    klseek(fd, 1025, SEEK_SET);
    if(kread(fd, buff2, strlen("hello world") + 1) == -1) {
        printk("error reading file\n");
    }
    if(strcmp(buff2, "hello world")) {
        printk("error reading file\n");
    } else {
        printk("successfully read file dup worked\n");
    }

    ksync();
    if(kfstat(fd, &stbuf) != -1) {
        printk("kfstat stbuf.st_size=%u stbuf.st_dev=%d\n",stbuf.st_size,stbuf.st_dev);
    }
    if(kstat("/bar", &stbuf) != -1) {
        printk("kstat stbuf.st_size=%u stbuf.st_dev=%d\n",stbuf.st_size,stbuf.st_dev);
    }

    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    // Expect to get an error if we change the permission
    // to write only and do a read-only open. Right
    // now its set up to succeed. This is for kchmod and kfchmod.
    if(kchmod("/bar", S_IWUSR) == -1) {
	    printk("error kchmod\n");
    }

    P();
    if((fd = kopen("/bar", O_RDONLY)) == -1) {
	    printk("success kopen O_RDONLY failed\n");
    } else {
        printk("error kopen O_RDONLY succeeded\n");
    }

    if((fd = kopen("/bar", O_WRONLY)) == -1) {
	    printk("error kopen O_WRONLY failed\n");
    }

    P();
    if(kfchmod(fd, S_IRUSR | S_IWUSR) == -1) {
	    printk("error kfchmod\n");
    }

    P();
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    P();
    if((fd = kopen("/bar", O_RDONLY)) == -1) {
	    printk("error kopen O_RDONLY\n");
    } else {
        printk("success\n");
    }

    P();
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    if(kunlink("/bar") == -1) {
        printk("kunlink:: error unlinking [/bar]\n");
    }

    if(kmkdir("/bar", S_IRWXU) == -1) {
        printk("kmkdir:: error creating dir [/bar]\n");
    }

    if(kchdir("/bar") == -1) {
        printk("error kchdir\n");
    }

    P();
    current = kget_current_dir_name();
    if(current && !strcmp(current,"/bar")) {
        printk("successfully retrieved current dir [kget_current_dir_name]\n");
    } else {
        printk("error retrieving current dir [kget_current_dir_name]\n");
    }

    P();
    if(kgetwd(current_buf)) {
        if(!strcmp(current_buf,"/bar")) {
            printk("successfully retrieved current dir [kgetwd]\n");
        } else {
            printk("error retrieving current dir [kgetwd]\n");
        }
    } else {
        printk("error retrieving current dir [kgetwd]\n");
    }

    P();
    if(kgetcwd(current_buf,10)) {
        if(!strcmp(current_buf,"/bar")) {
            printk("successfully retrieved current dir [kgetcwd]\n");
        } else {
            printk("error retrieving current dir [kgetcwd]\n");
        }
    } else {
        printk("error retrieving current dir [kgetcwd]\n");
    }

    P();
    if(kunlink("/foo") == -1) {
        printk("kunlink:: failed to unlink [/foo]\n");
    }

    if(kmkdir("/foo", S_IRWXU) == -1) {
        printk("kmkdir:: failed to mkdir [/foo]\n");
    }

    if((fd = kopen("/foo", O_RDONLY)) == -1) {
	    printk("success kopen O_RDONLY failed\n");
    }

    if(kfchdir(fd) == -1) {
        printk("error kchdir\n");
    }

    P();
    current = kget_current_dir_name();
    if(current && !strcmp(current,"/foo")) {
        printk("successfully retrieved current dir [kget_current_dir_name]\n");
    } else {
        printk("error retrieving current dir [kget_current_dir_name]\n");
    }

    P();
    if(kgetwd(current_buf)) {
        if(!strcmp(current_buf,"/foo")) {
            printk("successfully retrieved current dir [kgetwd]\n");
        } else {
            printk("error retrieving current dir [kgetwd]\n");
        }
    } else {
        printk("error retrieving current dir [kgetwd]\n");
    }

    P();
    if(kgetcwd(current_buf,10)) {
        if(!strcmp(current_buf,"/foo")) {
            printk("successfully retrieved current dir [kgetcwd]\n");
        } else {
            printk("error retrieving current dir [kgetcwd]\n");
        }
    } else {
        printk("error retrieving current dir [kgetcwd]\n");
    }

    timebuf.actime = 17;
    timebuf.modtime= 19;
    if(kutime("/bar", &timebuf) == -1) {
        printk("error setting kutime\n");
    } else {
        printk("successfully updated file time\n");
    }
    if(kstat("/bar", &stbuf) == -1) {
        printk("error stating file\n");
    } else {
        printk("successfully stat'd file\n");
    }
    if(timebuf.actime  == stbuf.st_atime &&
       timebuf.modtime == stbuf.st_mtime) {
        printk("successfully tested kutime\n");
    } else {
        printk("error kutime failed atime=%d mtime=%d\n",
              stbuf.st_atime,stbuf.st_mtime);
    }

    if((fd=kopen2("/star", O_CREAT | O_RDONLY, S_IRUSR)) == -1) {
        printk("error kopen2 failed\n");
    } else {
        printk("kopen2 succeeded\n");
    }

    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }

    if((fd = kcreat("/starbar", S_IRUSR | S_IWUSR)) == -1) {
    // if((fd = kopen("/starbar", O_CREAT | O_RDWR)) == -1) {
        printk("kcreat error creating file\n");
    } else {
        printk("kcreat successfully created file\n");
    }
    // Test kdup+, kdup2+, kfstat+, klstat (do when we test link), ksync+,
    // kchmod+, kfchmod+, kgetcwd+, kgetwd+, kget_current_dir_name+,
    // kchdir+, kfchdir+, kutime+
    // Test kopen2+, kcreat+
    // Test krename*
    if(kwrite(fd, "hello world", strlen("hello world")) == -1) {
        printk("error writing file\n");
    }
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }
    if(krename("/starbar", "/uranus") == -1) {
        printk("error krename file failed\n");
    }
    if((fd = kopen("/uranus", O_RDONLY)) == -1) {
        printk("error kopen file failed\n");
    }
    memset(buff,0x0,strlen("hello world") + 1);
    if(kread(fd, buff, 11) == -1) {
        printk("error reading file, krename failed\n");
    }
    if(!strcmp(buff, "hello world")) {
        printk("successfully read file, krename succeeded!!!\n");
    } else {
        printk("error reading file, krename failed [%s]\n",buff);
        if(kstat("/uranus", &stbuf) == -1) {
            printk("error stat'ing file /uranus\n");
        }
        printk("size=%u\n",stbuf.st_size);
    }
    if((fd = kclose(fd)) == -1) {
        printk("file_init:: error closing file\n");
    }
    printk("file_init:: success\n");
    return 0;
}
#endif
