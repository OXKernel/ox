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
//      block.h
//
// @description:
//      Buffer cache storing disk read/writes in RAM for
//      optimization.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _BLOCK_H
#define _BLOCK_H

typedef unsigned long block_t;

// The number of blocks stored.
//#define BLOCK_ARRAY_SIZE 4096 // RGD
#define BLOCK_ARRAY_SIZE 128 // RGD
#define BLOCK_FREE         -1
#define BLOCK_MAX_LRU    (block_t)4294967295 // 2147483647
#define BLOCK_NOPOS        -1

typedef enum block_rtvl {
   BLOCK_OK    = 0,
   BLOCK_FAIL  = -1,
   BLOCK_PARAM = -2
} block_rtvl_t;

block_rtvl_t block_open(char *path, int *dev);

block_rtvl_t block_close(int dev);

block_rtvl_t block_read(int dev, block_t block, char *data);

block_rtvl_t block_write(int dev, block_t block, char *data);

block_rtvl_t block_free(int dev, block_t block);

//
// block_sync:
// Go through entire cache, write every block to disk,
// reset dirty bit to be clean.
//
block_rtvl_t block_sync(int dev);

//
// block_disk_write:
//
// Force a disk write, normally a write means write to the cache.
//
block_rtvl_t block_disk_write(int dev, block_t block, char *data);

//
// block_disk_write_sync_cache:
//
// Force a disk write, normally a write means write to the cache,
// the difference here vs. 'block_disk_write' is that we keep the cache
// in sync if the block was no longer in the cache. In both functions
// we treat the user input as more current than cache.
//
block_rtvl_t block_disk_write_sync_cache(int dev, block_t block, char *data);

void block_reinit();

#endif
