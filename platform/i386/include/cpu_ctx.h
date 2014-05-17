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
 * Copyrght (C) Roger George Doss. All Rights Reserved
 ************************************************************
 *
 *      @module
 *             <platform/i386/cpu_ctx.h> 
 *
 *      @description
 *
 *		CPU Context as saved by the underlying
 *		assembler routines, in asm_core.
 *		These structure is used to access the
 *		state prior to the actual ( interrupt | exception | system call )
 *		handler running.  It is also restored when the handler
 *		finishes.
 *
 *		NOTE: This ABSOLUTELY MUST match how the context is
 * 		      pushed onto the stack via the macros CTX_SAVE, CTX_RESTORE.
 *		      see asm_core/common/ctx.inc
 *
 *      @author
 *              Roger George Doss
 */
#ifndef _PLATFORM_CPU_CTX_H
#define _PLATFORM_CPU_CTX_H 1

struct cpu_ctx {

	/* Stack as setup by assembler macro.
	 */
	int 	ds,
		es,
		fs,
		gs;

	/* Stack as setup by pushad. */
	long	edi,
		esi,
		ebp,
		esp,
		ebx,
		edx,
		ecx,
		eax;

	/* Stack as setup by Intel CPU when enterring
	 * an Interrupt. 
	 */
	long tmp;
	long eip;

	/* code segment
	 */
	int	cs;

	/* eflags, processor state
	 */
	long eflags;

	/* stack pointer and segments
	 */
	long sys_esp;
	int   ss;

};

#endif /* _PLATFORM_CPU_CTX_H */
