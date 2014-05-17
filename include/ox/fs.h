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
 * <ox/fs.h>
 *   file system header file
 *   This includes all of the file system headers from
 *   include/ox/fs.
 *
 *******************************************************/
#ifndef _OX_FS_H
#define _OX_FS_H  1
#ifdef __cplusplus
 extern "C" {
#endif
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/utime.h"
#include "fs/bitmap.h"
#include "fs/bool.h"
#include "fs/paths.h"
#include "fs/block.h"
#include "fs/dev.h"
#include "fs/inode.h"
#include "fs/file.h"
#include "dirent.h"
#include "fs/dir.h"
#include "fs/link.h"
#include "fs/fs_syscalls.h"
#include "fs/krealpath.h"
#ifdef __cplusplus
 }
#endif
#endif /* _OX_FS_H */
