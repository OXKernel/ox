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
 * <platform/i386/limits.h>
 *
 * system limits based on architecture
 *
 ********************************************************/
#ifndef _PLATFORM_LIMITS_H
#define _PLATFORM_LIMITS_H 1
#ifdef __cplusplus
 extern "C" {
#endif
#define CHAR_BIT   8     /* bits in char */
#define MB_LEN_MAX 6     /* multi-bit char */

/* signed values */
#define SCHAR_MAX  127
#define SCHAR_MIN  (-128)

#define SHRT_MAX   32767
#define SHRT_MIN   (-32768)

#define INT_MAX    2147483647
#define INT_MIN    (-2147483648)

#define LONG_MAX   2147483647L
#define LONG_MIN   (-2147483648L)
 
/* unsigned values */
#define UCHAR_MAX   255
#define USHRT_MAX   65535
#define UINT_MAX    4294967295U

#define ULONG_MAX   4294967295UL

/* char definition */
#define CHAR_MAX SCHAR_MAX
#define CHAR_MIN SCHAR_MIN

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_LIMITS_H */
