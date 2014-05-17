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
 * <sys/errno.h>
 * 
 *  POSIX Operating System error codes
 *   and OX extensions
 ********************************************************/
#ifndef _SYS_ERRNO_H
#define _SYS_ERRNO_H  1
extern int errno;

#ifdef __OX_NETWORKING__
#define ENERROR      58  /* maximum error values */
#else
#define ENERROR      41
#endif

#define EOK          0   /* OK */

#define EPERM        1   /* no permission */
#define ENOENT       2   /* no inode entry */
#define ESRCH        3   /* no such process */
#define EINTR        4   /* interupted system call */
#define EIO          5   /* i/o */
#define ENXIO        6   /* no such dev/address */
#define E2BIG        7   /* arg list too big */
#define ENOEXEC      8   /* exec format */
#define EBADF        9   /* bad file desc */
#define ECHILD       10  /* no child process */
#define EAGAIN       11  /* resource temp n/a */
#define ENOMEM       12  /* out of memory */
#define EACCES       13  /* no access/permission denied */
#define EFAULT       14  /* bad address/pointer */
#define ENOTBLK      15  /* not a block special file */
#define EBUSY        16  /* resource buisy */
#define EEXIST       17  /* file exists */
#define EXDEV        18  /* improber link */
#define ENODEV       19  /* no such device */
#define ENOTDIR      20  /* not a directory */
#define EISDIR       21  /* is a directory */
#define EINVAL       22  /* invalid arg */
#define ENFILE       23  /* too many open files in system */ 
#define EMFILE       24  /* too many open files */
#define ENOTTY       25  /* inapproperiate i/o operation */
#define ETXTBSY      26  /* not in use */
#define EFBIG        27  /* file too large */
#define ENOSPC       28  /* no space left on device */
#define ESPIPE       29  /* invalid seek */
#define EROFS        30  /* read only file system */
#define EMLINK       31  /* too many links */
#define EPIPE        32  /* broken pipe */
#define EDOM         33  /* domain error */
#define ERANGE       34  /* result too large */
#define EDEADLK      35  /* resource deadlock avoided */
#define ENAMETOOLONG 36  /* file name too long */
#define ENOLCK       37  /* no locks avail */
#define ENOSYS       38  /* not implemented */
#define ENOTEMPTY    39  /* directory not empty */
#define ELOOP        40  /* too many sym-links */

#ifdef __OX_NETWORKING__
#define EPACKSIZE    41  /* invalid packet size */
#define EOUTOFBUFS   42  /* no buffers avail */
#define EBADIOCTL    43  /* bad ioctl */
#define EBADMODE     44  /* bad mode */
#define EWOULDBLOCK  45  
#define EBADDEST     46  /* bad destination */
#define EDSTNOTRCH   47  /* destination not reachable */
#define EISCONN      48  /* connection already established */
#define EADDRINUSE   49  /* address in use */
#define ECONNREFUSED 50  /* connection refused */
#define ECONNRESET   51  /* connection reset */
#define ETIMEDOUT    52  /* timed out */
#define EURG         53  /* urgent data */
#define ENURG        54  /* no urgent data */
#define ENOTCONN     55  /* not connected */
#define ESHUTDOWN    56  /* write on shutdown connection */
#define ENOCONN      57  /* no connection */
#endif 

#define EGEN         58  /* Generic error */

/* layout internal kernel 
 * errors here
 */
#endif /* _SYS_ERRNO_H */
