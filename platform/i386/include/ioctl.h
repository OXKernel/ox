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
 * <platform/i386/ioctl.h>
 * 
 *   system dependent encoding for i/o control
 *     This system call is primarily used to access
 *     device drivers by issuing requests to the
 *     drivers. Some of the calls are standard, and are 
 *     a part of POSIX, others are specific to a given
 *     flavor of UNIX. All ioctls depend on the types of devices
 *     supported, and their respective drivers.
 *     ioctl is an exception to the general UNIX rule,
 *     that the system does not destinquish between the
 *     various types of files, because the type of device
 *     special file for which we are issuing requests, determines
 *     which requests we can issue. All devices fall in either
 *     of two categories, block or character, with some
 *     supporting both interfaces.
 *
 ********************************************************/
#ifndef _PLATFORM_i386_IOCTL_H
#define _PLATFORM_i386_IOCTL_H  1
#ifdef __cplusplus
 extern "C" {
#endif
/*  
 * ioctl requests have the following
 * format on x86 architecture (32bit/little-endian):-
 *
 * bits 31,30 -> mode [void,read,write,read/write]
 * bits 29,16 -> size of requests  [0 - 2 ^ 14 - 1]
 * bits 15,8  -> request type      [0 - 2 ^ 8 - 1]
 * bits 7,0   -> request number    [0 - 2 ^ 8 - 1]
 *
 * requests have a total range of  [0 - 2 ^ 16 -1]
 * these are divided into two fields:-
 * 
 * request type   -> indicates the device type
 * request number -> indicates operation on that device
 */
#define _IOC_V   0U   /* void  */
#define _IOC_R   1U   /* read  */
#define _IOC_W   2U   /* write */
#define _IOC_RW  3U   /* read/write */

/* size of bit fields */
#define _IOC_MODE_BIT 2
#define _IOC_SIZE_BIT 14
#define _IOC_TYPE_BIT 8
#define _IOC_NMBR_BIT 8

/* numeric masks used for encoding/decoding */
#define _IOC_MODE_MASK ((1 << _IOC_MODE_BIT ) - 1)
#define _IOC_SIZE_MASK ((1 << _IOC_SIZE_BIT ) - 1)
#define _IOC_TYPE_MASK ((1 << _IOC_TYPE_BIT ) - 1)
#define _IOC_NMBR_MASK ((1 << _IOC_NMBR_BIT ) - 1)

/* number of bits to shift for each field */
#define _IOC_MODE_SHIFT  (_IOC_TYPE_BIT + _IOC_NMBR_BIT + _IOC_SIZE_BIT) /* 30 */
#define _IOC_SIZE_SHIFT  (_IOC_TYPE_BIT + _IOC_NMBR_BIT)                 /* 16 */
#define _IOC_TYPE_SHIFT  (_IOC_TYPE_BIT)                                 /* 8  */
#define _IOC_NMBR_SHIFT  0

#define _IOC_ENCODE(mode,size,type,nmbr)   \
            (((mode) << _IOC_MODE_SHIFT) | \
             ((size) << _IOC_SIZE_SHIFT) | \
             ((type) << _IOC_TYPE_SHIFT) | \
             ((nmbr) << _IOC_NMBR_SHIFT))

/* macros to encode IOC numbers */
#define _IO  (type,nmbr)      _IOC_ENCODE(_IOC_V,            0,(type),(nmbr))
#define _IOR (type,nmbr,size) _IOC_ENCODE(_IOC_R, sizeof(size),(type),(nmbr))
#define _IOW (type,nmbr,size) _IOC_ENCODE(_IOC_W, sizeof(size),(type),(nmbr))
#define _IOWR(type,nmbr,size) _IOC_ENCODE(_IOC_RW,sizeof(size),(type),(nmbr))

/* macros to decode IOC numbers */
#define _IOC_MODE(rgst)  (((rgst) >> _IOC_MODE_SHIFT) & _IOC_MODE_MASK)
#define _IOC_SIZE(rgst)  (((rgst) >> _IOC_SIZE_SHIFT) & _IOC_SIZE_MASK)
#define _IOC_TYPE(rgst)  (((rgst) >> _IOC_TYPE_SHIFT) & _IOC_TYPE_MASK)
#define _IOC_NMBR(rgst)  (((rgst) >> _IOC_NMBR_SHIFT) & _IOC_NMBR_MASK)

#define _IOC_IN          (_IOC_W  << _IOC_MODE_SHIFT)
#define _IOC_OUT         (_IOC_R  << _IOC_MODE_SHIFT)
#define _IOC_INOUT       (_IOC_RW << _IOC_MODE_SHIFT)
#define _IOCSIZE_MASK    (_IOC_SIZE_MASK << _IOC_SIZE_SHIFT)
#define _IOCSIZE_SHIFT   (_IOC_SIZE_SHIFT)

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_i386_IOCTL_H */
