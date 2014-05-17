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
 * <sys/types.h>
 *
 * typedefs and defines for the system as defined in:-
 *   POSIX Standard: 2.6 Primitive System Data Types
 *
 *******************************************************/
#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* POSIX */
#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef _SIZE_T
#define _SIZE_T 
typedef unsigned long size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef long ssize_t;
#endif

#ifndef _CLOCK_T
#define _CLOCK_T
typedef long clock_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef unsigned long time_t;
#endif
 
#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t; /* signed difference of two pointers */
#endif

/* all types are defined using the
 * typedef's [u|s]type_t
 */
typedef unsigned long int utype_t;
typedef long int          stype_t;
typedef unsigned long long int  utype64_t;
typedef signed long long int    stype64_t;

typedef int blksize_t;  /* These are for the file system. */
typedef unsigned int blkcnt_t;

typedef  utype_t  dev_t;      /* device major/minor number */
typedef  utype_t  uid_t;      /* user id */
typedef  utype_t  gid_t;      /* group id */
typedef  utype_t  ino_t;      /* inode number */
typedef  utype_t  mode_t;     /* file mode, permissions */
typedef  utype_t  nlink_t;    /* number of links to a file */
typedef  utype_t  off_t;      /* offset within a file */
typedef  stype_t  pid_t;      /* process id */
typedef  utype_t  zone_t;     /* zone */
typedef  utype_t  block_t;    /* block */
typedef  utype_t  bit_t;      /* bit in bitmap */
typedef  utype_t  zone1_t;    /* zone number for v1 fs */
typedef  utype_t  bitchunk_t; /* n bits in bitmap */
typedef  utype64_t ino64_t;   /* 64 bit type. */
typedef  utype64_t off64_t;   /* 64 bit type. */

typedef  utype_t  key_t;
typedef  int      daddr_t;
typedef  char     *caddr_t;   /* char address */

struct ustat {
        daddr_t f_tfree;
        ino_t f_tinode;
        char f_fname[6];
        char f_fpack[6];
};

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_TYPES_H */
