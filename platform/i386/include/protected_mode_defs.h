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
 *  <platform/i386/protected_mode_defs.h>
 *	
 *	defines for protected_mode
 *
 *******************************************************/
#ifndef _PLATFORM_I386_PROTECTED_MODE_DEFS_H
#define _PLATFORM_I386_PROTECTED_MODE_DEFS_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* defines
 */ 
#define Nr_RES_GDT	8	/* number of entries reserved in GDT */
#define Nr_GDT		8192	/* number of gdt entries allowable by Intel hardware */
#define Nr_IDT   	255	/* number of idt entries allowable by Intel hardware */

#define Nr_PROCESS 	((Nr_GDT - Nr_RES_GDT)/2)	/* number of process */
#define Nr_LDT		Nr_PROCESS			/* number of LDT's   */

#define Nr_PROC_LDT 	3	/* number of segments in a LDT for a given process 
				 * 0 == NULL, 1 cs, 2 ds,ss 
				 */

	 			/* see ./kernel/platform/asm_core/{interrupt|exception}.s */
	 			/* see ./kernel/platform/{interrupt|exception}.c */
#define Nr_EXCPT_HNDLR	19	/* # of exception handlers */
#define Nr_INT_HNDLR    16	/* # of interrupt handlers, equal to ./platform/interrupt.h/Nr_IRQ */
#define SYSCALL_INT	80	/* system call interrupt 	*/

#define KERNEL_ACCESS   0x0
#define USER_ACCESS	0x3

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_PROTECTED_MODE_DEFS_H */
