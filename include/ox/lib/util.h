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
/**************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 *	<ox/lib/util.h>
 *
 *		misc routines
 *
 **************************************************************/
#ifndef _OX_LIB_UTIL_H
#define _OX_LIB_UTIL_H 1
#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG() printk("line (%d), file (%s)",__LINE__,__FILE__)

#define MAX(a,b) ((a) > (b))?(a):(b)
#define MIN(a,b) ((a) < (b))?(a):(b)

inline
void
to_cp      ( char *buf, wchar_t *widech );

inline
void
cl_buf     ( char *buf );

inline
void
cl_bufn    ( char *buf, size_t size );

inline
void
fl_buf     ( char *buf, char fill );

inline
void
fl_bufn    ( char *buf, char fill, size_t size );

#ifdef __cplusplus
 }
#endif
#endif /* _OX_LIB_UTIL_H */
