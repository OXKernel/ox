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
 * @module  segment_defs.h
 * @description
 * 	contains special defines for initializing segment
 * 	descriptors under Intel
 */
#ifndef _PLATFORM_I386_SEGMENT_DEFS_H
#define _PLATFORM_I386_SEGMENT_DEFS_H 1
#ifdef __cplusplus 
 extern "C" {
#endif


/* special values, see Intel Developer manual, vol 3, 3-13
 * table 3-1, these values are used to initialize various
 * types of descriptors, used by kernel and processor
 */
#define _DESC_EXECREADONLY	2
#define _DESC_DEFAULT_OP 	1	/* 32 bit segment 	*/
#define _DESC_READWRITE		2
#define _DESC_GRANULARITY	1	/* 4k bytes		*/
#define _DESC_CODESEG		8	/* indicates code seg	*/
#define _DESC_DATASEG		0	/* indicates data seg	*/
#define _DESC_CSSEG		1	/* code or data seg	*/
#define _DESC_SYSSEG		0	/* system segment	*/
#define _DESC_PRESENTF		1	/* indicates present	*/
#define _DESC_EXPANDDOWN	4	/* stack		*/
#define _DESC_INTERRUPT	       14	/* interrupt gate	*/
#define _DESC_TRAP	       15	/* trap gate		*/
#define _DESC_TSS		9	/* TSS, not busy	*/
#define _DESC_AVAILABLE		1	/* available for use	*/
#define _DESC_KERNPRIV		0	/* KERNEL privaledge	*/
#define _DESC_USERPRIV		3	/* USER   privaledge	*/
#define _DESC_LDT	        2	/* LDT descriptor	*/
	 
#ifdef __cplusplus
  }
#endif
#endif /* _PLATFORM_I386_SEGMENT_DEFS_H */
