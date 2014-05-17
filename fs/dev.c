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
// TODO - change this to call ata_read, ata_write
//      - lseek is a no-op
//      - dev_open likely will also be a no-op
//        just return dev = 0 for primary drive
//
// @file:
//      dev.c
//
// @description:
//      Low level device interface. For testing in user space,
//      we read/write in sector blocks from a top a file system.
//      In kernel mode, we make the actual calls to a hard disk
//      via a driver.
//
//      NOTE: This code was tested as part of testing block.c.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#include "block.h"
#include "dev.h"

#ifdef _USER_SPACE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
//#include "pio.h"
#include <drivers/block/pio.h>
#endif

dev_rtvl_t dev_open(char *path, int *dev)
{
#ifdef _USER_SPACE
   if(!path || !dev) {
      return DEV_PARAM;
   }
   (*dev) = open(path, O_RDWR | O_CREAT, S_IRWXU); 
   if(*dev < 0) {
      *dev = DEV_FAIL;
      return DEV_FAIL;
   }
   return DEV_OK;
#else
    // This is temporary, we need to mount properly
    // based on the path and mount table.
    // Right now, we check if there are any
    // ATA drives and assign the first one (primary).
    if(ata_nr_drives() > 0) {
        *dev = 0;
    }
    return DEV_OK;
#endif
}

dev_rtvl_t dev_close(int dev)
{
#ifdef _USER_SPACE
   if(dev < 0) {
      return DEV_PARAM;
   }
   close(dev);
   return DEV_OK;
#else
    return DEV_OK;
#endif
}

dev_rtvl_t dev_read(int dev, block_t block, char *data)
{
#ifdef _USER_SPACE
   if(dev < 0 || !data) {
      return DEV_PARAM;
   }
   if(read(dev, data, DEV_BLOCK_SIZE) < 0) {
      return DEV_FAIL;
   }
   return DEV_OK;
#else
    // Do a PIO read of the drive.
    if(!ata_read(dev, block, data)) {
        return DEV_OK;
    } else {
        return DEV_FAIL;
    }
#endif
}

dev_rtvl_t dev_write(int dev, block_t block, char *data)
{
#ifdef _USER_SPACE
   if(dev < 0 || !data) {
      return DEV_PARAM;
   }
   if(write(dev, data, DEV_BLOCK_SIZE) < 0) {
      return DEV_FAIL;
   }
   return DEV_OK;
#else
    // Do a PIO write of the drive.
    if(!ata_write(dev, block, data)) {
        return DEV_OK;
    } else {
        return DEV_FAIL;
    }
#endif
}

dev_rtvl_t dev_scan(int dev, block_t block)
{
#ifdef _USER_SPACE
   if(dev < 0 || block < 0) {
      return DEV_PARAM;
   }
   if(lseek(dev, block * DEV_BLOCK_SIZE, 0) < 0) {
      return DEV_FAIL;
   }
   return DEV_OK;
#else
   // The PIO code will seek the drive in the read/write.
   return DEV_OK;
#endif
}
