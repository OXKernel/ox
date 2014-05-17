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
/*
 * @module:
 *
 * 	<ox/asm_core/atom.h>
 *
 * @description:
 *
 * 	Interface for Assembly atomic operations.
 *
 * @author:
 *
 * 	Roger G. Doss
 */
#ifndef _PLATFORM_I386_ASM_CORE_ATOM_H
#define _PLATFORM_I386_ASM_CORE_ATOM_H 1

/*
 * atom_bit_set:
 * 
 * 	Returns 0 or 1 depending on value of the bit prior to
 * 	modification.
 */ 	
extern unsigned long 
atom_bit_set(volatile unsigned long *data, unsigned long index );

/*
 * atom_bit_clear:
 * 
 * 	Returns 0 or 1 depending on value of the bit prior to
 * 	modification.
 */ 	
extern unsigned long 
atom_bit_clear(volatile unsigned long *data, unsigned long index );

extern void 
atom_increment( volatile unsigned long *data );

extern void 
atom_decrement( volatile unsigned long *data );

/*
 * atom_inc_test:
 *
 * 	Returns 0 if *data non-zero
 * 	Returns 1 if *data is zero
 */
extern unsigned long 
atom_inc_test ( volatile unsigned long *data );

/*
 * atom_dec_test:
 *
 * 	Returns 0 if *data non-zero
 * 	Returns 1 if *data is zero
 */ 	 
extern unsigned long
atom_dec_test ( volatile unsigned long *data );

/*
 * atom_set:
 *
 * 	Returns the original *data prior to modification.
 */
extern unsigned long 
atom_set      ( volatile unsigned long *data, unsigned long value );

extern void
atom_add      ( volatile unsigned long *data, unsigned long value );

extern void
atom_dec      ( volatile unsigned long *data, unsigned long value );

#endif
