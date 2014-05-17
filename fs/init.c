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
//      init.c
// 
// @description:
//      Initialize the file system on a given device.
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
#include "init.h"

#include <drivers/block/pio.h>
// #include <ox/pio.h>

/*
 * fs_reset:
 *
 * This will reset the filesystem and force
 * the fs_init code to re-initialize the filesystem.
 *
 */
void fs_reset()
{
    block_t dev = 0;
    char zero_data[DEV_BLOCK_SIZE] = {0};
    char *path = "/";
    dev_open(path, &dev);
    dev_write(dev, BLOCK_START, zero_data);
}

/*
 * fs_init:
 *
 * Initialize the file system.
 * Returns 0 success
 *         1 error
 *
 */
int fs_init(unsigned long requested_size)
{
    /* Get the drive size before initializing.
     */
    master_inode_t *master = NULL;
    unsigned long start_sector = 0;
    unsigned long disk_size = 0;
    char *path = "/";
    int   dev  =  0;
    inode_rtvl_t rtvl = INODE_OK;

    disk_size = ata_disk_size(0, &start_sector);

    if(!disk_size) {
        printk("fs_init:: error sizing disk\n");
        return FS_INIT_FAIL;
    }

    rtvl = inode_dev_open(path, &dev);
    if(rtvl == INODE_INIT) {
       printk("fs_init:: calling inode_mkfs\n");
       if(requested_size < disk_size) {
            disk_size = requested_size;
       }
       if(inode_mkfs(path, disk_size) != INODE_OK) {
            printk("fs_init:: error initializing filesystem\n");
            return FS_INIT_FAIL;
       }
       if(inode_dev_open(path, &dev) != INODE_OK) {
            printk("fs_init:: error opening device\n");
            return FS_INIT_FAIL;
       }
    } else if(rtvl == INODE_OK) {
        printk("fs_init:: successfully intialized drive\n");
        master = master_get(dev);
        master_set_dev(path, dev);
        return FS_INIT_OK;
    } else {
        // rtvl == INODE_FAIL
        printk("fs_init:: error opening device\n");
        return FS_INIT_FAIL;
    }

}/* fs_init */
