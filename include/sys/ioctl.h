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
 *  <sys/ioctl.h>
 *   i/o control
 *  <platform/machine/ioctl.h>
 *   defines necessary macros for encoding
 *   ioctls for a given architecture
 ********************************************************/
#ifndef  _SYS_IOCTL_H
#define  _SYS_IOCTL_H  1
#ifdef __cplusplus
 extern "C" {
#endif
#include <platform/ioctl.h>
#include <sys/termios.h>

/* ioctl's for terminal */
#define TCGETS     _IOR('T', 8, struct termios) /* tcgetattr */
#define TCSETS     _IOW('T', 9, struct termios) /* tcsetattr, TCSANOW */
#define TCSETSW    _IOW('T',10, struct termios) /* tcsetattr, TCSADRAIN */
#define TCSETSF    _IOW('T',11, struct termios) /* tcsetattr, TCSAFLUSH */
#define TCSBRK     _IOW('T',12, int)            /* tcsendbreak */
#define TCDRAIN    _IO ('T',13)                 /* tcdrain */
#define TCFLOW     _IOW('T',14, int)            /* tcflow */
#define TCFLSH     _IOW('T',15, int)            /* tcflush */ 
#define TCIOCGPGRP _IOW('T',18, int)
#define TCIOCSPGRP _IOW('T',19, int)

#ifdef __cplusplus
 }
#endif
#endif /* _SYS_IOCTL_H */
