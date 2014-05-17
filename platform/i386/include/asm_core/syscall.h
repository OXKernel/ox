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
 * Copyright(C) Roger George Doss. All Rights Reserved.
 ************************************************************
 * 
 * @module  <platform/i386/asm_core/syscall.h
 *	    
 *          contains code for kernel syscall operation
 *	    these are defined in arch/i386/asm_core/syscall.s
 *	    and are platform specific
 */
#ifndef _PLATFORM_I386_ASM_CORE_SYSCALL_H
#define _PLATFORM_I386_ASM_CORE_SYSCALL_H 1
#ifdef __cplusplus
extern "C" {
#endif

extern
void syscall_handler( void );

extern
void syscall_handler_return( void );

extern
void syscall_signal_return( void );

extern
void soft_irq( void );

extern
void rescheduler( void );

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_ASM_CORE_SYSCALL_H */
