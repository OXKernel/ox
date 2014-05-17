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
/************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved
 ************************************************************
 *
 *      @module
 *             <platform/i386/segment_selectors.h>
 *
 *      @description
 *		defines segment selectors for segment
 *		descriptors in the GDT/LDT's
 *		
 *      @author
 *              Roger George Doss
 */
#ifndef _PLATFORM_SEGMENT_SELECTORS_H
#define _PLATFORM_SEGMENT_SELECTORS_H 1

/* 
 * also, defined in asm_core/common/cdef.inc
 * for NASM code
 */ 
#define KERNEL_CS               0x10	/* second GDT entry */
#define KERNEL_DS		0x18	/* third  GDT entry */
#define USER_CS                 0x23	/* fourth GDT entry */
#define USER_DS			0x2B	/* fifth  GDT entry */

#endif	/* PLATFORM_SEGMENT_SELECTORS_H */
