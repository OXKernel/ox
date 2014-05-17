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
/*********************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *********************************************************************
 *
 * 	<ox/lib/bit.h>
 *
 *		generic get and set bit routines
 *********************************************************************/
#ifndef _OX_LIB_BIT_H
#define _OX_LIB_BIT_H 1
#ifdef __cplusplus
 extern "C" {
#endif

/*
 * getBits:-
 * get the n bits at position p from x
 */
inline
unsigned getBits(unsigned x,int p,int n);

/* 
 * setBits:-
 * set n bits at position p in x from left
 * most bits in y
 */
inline
unsigned setBits(unsigned x, int p, int n, unsigned y);

/* 
 * setBits_p:-
 * set n bits at position p in x from left
 * most bits in y. This version requires a
 * pointer to the number.
 * NOTE: we added additional parenthesis around
 * 	 operator|.
 */
inline
void setBits_p(unsigned long *x, int p, int n, unsigned y);

#ifdef __cplusplus
 }
#endif
#endif /* _OX_LIB_BIT_H */
