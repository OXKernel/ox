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
 *	<ox/lib/conversions.h>
 *
 **************************************************************/
#ifndef _OX_LIB_CONVERSIONS_H
#define _OX_LIB_CONVERSIONS_H 1
#ifdef __cplusplus
extern "C" {
#endif

/* valid bases
 * NOTE: bases larger than hex
 * are not supported.
 */
#define BIN 0x2
#define DEC 0xA
#define OCT 0x8
#define HEX 0x10

/* convert from string to integer */
unsigned long 
strtoul( const char *charp, char **endptr, int base);

long 
strtol ( const char *charp, char **endptr, int base);

int
atoi   ( char *charp );

/* convert from integer to string */
int
ultostr( char *buf, unsigned long val, int base);

int
ltostr ( char *buf, long val, int base );

int
itoa   ( char *buf, int  val );

#ifdef __cplusplus
 }
#endif
#endif /* _OX_LIB_CONVERSIONS_H */
