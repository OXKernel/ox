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
/******************************************************** 
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 *
 * <dirent.h>
 *
 * Directory access stuctures.
 *
 ********************************************************/

#ifndef _DIRENT_H
#define _DIRENT_H

struct dirent
{
#ifndef __USE_FILE_OFFSET64
    ino_t d_ino;
    off_t d_off;
#else
    ino64_t d_ino;
    off64_t d_off;
#endif
unsigned short int d_reclen;
unsigned char d_type;
char d_name[MAX_PATH];
};

struct __dirstream
{
    int     __fd;	            /* File descriptor. */
    inode_t __data;	            /* Directory block. */
    size_t  __allocation;	    /* Space allocated for the block. */
    size_t  __offset;	        /* Current offset into the block. */
    size_t  __block_ptr;	    /* The current block we are in.   */
    block_t __block;            /* Actual block number. */
    char    __path[MAX_PATH];   /* path to this dir.    */
    struct dirent __entry;      /* Filled by readdir.   */
};

typedef struct __dirstream DIR;

#endif // _DIRENT_H
