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
 * @module
 * 	bit.c
 *
 *	Generic get and set bit routines.
 *
 *********************************************************************/
#include <ox/lib/bit.h>
#include <ox/linkage.h>

/*
 * getBits:-
 * get the n bits at position p from x
 */
__clinkage__
inline
unsigned getBits(unsigned x,int p,int n)
{
	return ((x >> (p+1-n)) & ~(~0 << n));
}

/* 
 * setBits:-
 * set n bits at position p in x from left
 * most bits in y
 */
__clinkage__
inline
unsigned setBits(unsigned x, int p, int n, unsigned y)
{
	return (x & ~(~(~0 << n) << (p+1-n)) |
		(y & ~(~0 << n)) << (p+1-n));
}

/* 
 * setBits_p:-
 * set n bits at position p in x from left
 * most bits in y. This version requires a
 * pointer to the number.
 * NOTE: we added additional parenthesis around
 * 	 operator|.
 */
__clinkage__
inline
void setBits_p(unsigned long *x, int p, int n, unsigned y)
{
	*x =  ((*x & ~(~(~0 << n) << (p+1-n))) |
                ((y & ~(~0 << n)) << (p+1-n)));
}

/*
 * EOF
 */ 
