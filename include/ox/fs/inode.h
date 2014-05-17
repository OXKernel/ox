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
//      inode.h
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
#ifndef _INODE_H
#define _INODE_H

#define MAX_DEV 256
#define INODE_MASTER_BLOCK 0
#define INODE_ROOT_BLOCK   master->inode_start
#define INODE_NULL         0
#define INODE_BLOCKS       54   // These were hard code to 4096 sector, its 512.
#define INODE_PAD          166  // 54*4-2 (for is_symlink,is_hardlink fields)
#define BMAP_BLOCKS        127  // INODE_BLOCKS is the size of array of blocks in inode
#define MNODE_PAD          436  // BMAP_BLOCKS is size of array of blocks:
                                // calculated as: ((DEV_BLOCK_SIZE/sizeof(block_t))-1)

#define INODE_MAGIC       1925 // Identifies our file system on disk.
#define INODE_NR_PASS        2
#define INODE_NOPOS         -1
#define INODE_NR_DEV        16 // Max number of devices we currently support.

#define BLOCK_START 4096
#define TWOMEG      2097152

// RGDTODO
// This should be 64 bit, we will look at the code
// in file.c and dir.c to see if we can implement it.
// Otherwise, the maximum file size is 4GB even though
// the max file system size is 2TB. For file systems
// larger than that, we are hopingwe could address them
// with a 4096 DEV_BLOCK_SIZE which would give us 16TB.
// It really makes no sense to me to have a 512 byte sector
// for a multi-terabyte drive. We could implement it by
// addressing 8 512 byte blocks in succession in the dev.c
// code but ideally we want support in the BIOS and disk drive.
// This is something to be done later.
typedef unsigned long long int inode_ptr_t;
typedef unsigned int inode_group_t; // Group.
typedef unsigned int inode_own_t;   // Owner.
typedef unsigned int inode_perm_t;  // Permissions.

typedef enum inode_mode {
    INODE_INVALID_MODE      = -1,
    INODE_CREATE_FILE       = 1,
    INODE_CREATE_DIR        = 2,
    INODE_CREATE_SYMLINK    = 3,
    INODE_CREATE_HARDLINK   = 4,
    INODE_READ              = 5,
    INODE_WRITE             = 6,
    INODE_EXECUTE           = 7,
    INODE_RW                = 8,  // Read/Write
    INODE_RX                = 9,  // Read/Execute
    INODE_WX                = 10, // Write/Execute
    INODE_RWX               = 11, // Read/Write/Execute
    INODE_DONTCHECKPERMS    = 12  // Don't check permissions
} inode_mode_t;

// NOTE: This scheme allows us to address 512 bytes per sector times 4Gig sectors
//       or 2048 gigs (2 Terabytes). This should be sufficient for a first
//       version file system. A mere 4096 512 sectors is 2 meg.
//       In the future, I would like to see a 4096 or 8192 sector.
typedef struct master_inode {
   short magic;           /* Magic to identify the file system. */
   short init_fs;         /* 1 if fs is initialized, 0 otherwise. */
   block_t block_start;       /* location of master inode. */
   block_t inode_map_start;   /* map of free inodes start. */
   block_t inode_map_end;     /* map of free inodes end.   */
   block_t bmap_map_start;    /* map of free blocks start. */
   block_t bmap_map_end;      /* map of free blocks end.   */
   block_t inode_start;       /* start of inodes.          */
   block_t inode_end;         /* end of inodes.            */
   block_t data_start;        /* start of data.            */
   block_t data_end;          /* end of data.              */
   block_t imap_ptr;          /* what inode map block on disk.  */
   block_t imap_bit;          /* what inode map bit. */
   block_t bmap_ptr;          /* what bit map block on disk.    */
   block_t bmap_bit;          /* what bit map bit. */
   block_t inodes;            /* number of inodes. */
   block_t blocks;            /* number of blocks. */
   char *imap;            /* Created on the fly and loaded. */
   char *bmap;            /* Created on the fly and loaded. */
   block_t dev;      /* Device we are on. */
   char pad[MNODE_PAD];   /* padding to fit page size. */
} master_inode_t;

typedef struct inode {
   char user_read;
   char user_write;
   char user_execute;

   char group_read;
   char group_write;
   char group_execute;

   char world_read;
   char world_write;
   char world_execute;

   char is_directory; /* If this is a directory, 
                       * all blocks point to other inodes, including next. */
   char is_device;    /* If this is a device, call device driver subsystem. */
   char is_file;      /* If this is a file, all blocks point to data, including next. */
   char is_symlink;   /* If this is a symlink.  */
   char is_hardlink;  /* If this is a hardlink. */

   block_t  create_time;   /* Time of last status change. */
   block_t  modified_time; /* Time of last modification.  */
   block_t  accessed_time; /* Time of last access.        */

   block_t  self;
   block_t  parent;

   /* The following are used for implementing files:
    * size      (sizeof file)
    * pos       (position in the file)
    * current   (current block number of block we are in)
    * dev       (device we are on)
    */ 
   inode_ptr_t  size; /* Size of the file in 64 bits. */

   inode_ptr_t  pos;  /* Where in the file we are currently. */

   // NOTE - These fields must be initialized on open of the file.
   block_t      current; /* Current data block. */
   block_t      current_parent; /* Block map to which current is stored in. */
   int          iblock; /* Where in the current_parent we are. */
   int              dev; /* Device we are on. */
   inode_perm_t  o_mode; /* File open mode. */
   // The above fields are needed for implementing files.

   inode_group_t group; /* Group where we belong. */
   inode_own_t   owner; /* Who owns this file. */

   block_t refcount;    /* Reference count. */

   char path[MAX_PATH];

   /* block_t blocks[INODE_BLOCKS]; */
   char pad[INODE_PAD];
   block_t next; /* Map to blocks in case of a regular file. */

   // RGDTODO - Do we need to adjust INODE_BLOCKS and add
   //           a pointer to bmap here or do we add
   //           it in the function as a stack object and simply read
   //           the next pointer ? Incidently, the data will come
   //           from the buffer cache and that we should init to 0
   //           and end should be zero. See addr.c for how to proceed.
} inode_t;

typedef struct block_map {
   block_t next;
   block_t blocks[BMAP_BLOCKS];
} block_map_t;

typedef struct link {
  char path[MAX_PATH];
  char pad[MAX_PATH];
} link_t;

typedef enum inode_rtvl {
   INODE_OK             = 0,
   INODE_FAIL           = -1,
   INODE_PARAM          = -2,
   INODE_FILE_NOT_FOUND = -3,
   INODE_INVALID_PATH   = -4,
   INODE_EXISTS         = -5,
   INODE_INCONSISTENT   = -6,
   INODE_INIT           = -7
} inode_rtvl_t;

inode_rtvl_t inode_mkfs(char *path, block_t size);
inode_rtvl_t inode_create_fs(char *path, block_t block_start, block_t size);
inode_rtvl_t inode_dev_open(char *path, int *dev);
inode_rtvl_t inode_dev_close(int dev);
inode_rtvl_t inode_get(int dev, 
                       block_t current_dir, 
                       char *path, 
                       bool do_readlink, /* true => readlink, false => get the link itself */
                       inode_mode_t mode,
                       inode_t *inode);
inode_rtvl_t inode_create(int dev,
                          block_t current_dir,  // cwd
                          char *path,           // path of node to create
                          inode_mode_t  mode,   // mode in which we are to create the node
                          inode_perm_t  perm,   // desired permissions
                          inode_perm_t  umask,  // umask
                          inode_group_t group,  // our group
                          inode_own_t   owner,  // our owner
                          char *lpath);         // for creating sym and hard links
inode_rtvl_t inode_free(int dev,block_t current_dir,char *path,inode_t *newnode);

inode_rtvl_t inode_get_data_block(int dev, block_t *block);
inode_rtvl_t inode_get_inode_block(int dev, block_t *block);

inode_rtvl_t inode_free_data_block(int dev, block_t block);
inode_rtvl_t inode_free_inode_block(int dev, block_t block);

// Utility methods for getting the block and parent block numbers
// given a path and current_dir.
block_t inode_get_block_number(int dev, block_t current_dir, char *path);

block_t inode_get_parent_block_number(int dev, block_t current_dir, char *path);

inode_perm_t inode_get_permissions(inode_t *inode);
inode_rtvl_t inode_set_permissions(inode_t *inode, inode_perm_t perm, inode_perm_t umask);

// Convert unix permissions to ox. Checking all permissions.
inode_mode_t inode_get_mode_from_all(inode_perm_t perm,
                            inode_perm_t umask);

// Convert unix permissions to ox (just looking at user).
inode_mode_t inode_get_mode(inode_perm_t perm,
                            inode_perm_t umask);

// Return true or false if we can perform the operation 
// requested based on the input.
bool inode_check_permissions(inode_t *inode, 
                             inode_perm_t perm, 
                             inode_perm_t umask, 
                             inode_group_t group, 
                             inode_own_t owner,
                             inode_mode_t mode);

inode_rtvl_t inode_set_parent_mod_time(int dev, block_t parent);
inode_rtvl_t inode_set_timestamps(inode_t *inode);

// Given a path set the device.
// Return -1 if failed to insert into table, otherwise
// i indicating successful insert.
int master_set_dev(char *path, int dev);

// Given a path return the associated device.
// Return -1 if failed to retrieve from table, otherwise
// the device number.
int master_get_dev(char *path);

#endif
