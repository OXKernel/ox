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
//      init.h
//      #include <ox/fs/init.h>
//
// @description:
//      Initialize the file system on a given device.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _FS_INIT_H
#define _FS_INIT_H

#define FS_INIT_OK      0
#define FS_INIT_FAIL    1

/*
 * fs_reset:
 *
 * This will reset the filesystem and force
 * the fs_init code to re-initialize the filesystem.
 *
 */
void fs_reset();

/*
 * fs_init:
 *
 * Initialize the file system.
 * Returns 0 success
 *         1 error
 *
 */
int fs_init(unsigned long requested_size);

#endif
