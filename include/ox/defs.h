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
/*************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *************************************************************
 * 	<ox/defs.h>
 *          contains defines for ox kernel
 * @TODO -
 *          See fs for file system defines, I think Nr_FILES_OPEN
 *          should be checked for use.
 */
#ifndef _OX_DEFS_H
#define _OX_DEFS_H  1
#ifdef __cplusplus
 extern "C" {
#endif

#define Nr_PRIORITY	32	/* number of priority queues */
#define Nr_SYS_CALL 	87	/* number of system calls, see call.h */
#define Nr_FILES_OPEN 	128	/* number of open files a process can have */

#ifdef  __OX_64BIT__
#define Nr_DIGITS 22		/* number of digits in an integer */
#else
#define Nr_DIGITS 11
#endif

#ifdef __cplusplus 
 }
#endif
#endif  /* _OX_DEFS_H */
