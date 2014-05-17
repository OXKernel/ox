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
 *             <platform/i386/ptrace.h> 
 *
 *      @description
 *
 *	Defines for ptrace.
 *	Offsets into the cpu_ctx are 
 *	dependent on the cpu_ctx as
 *	defined in cpu_ctx.h.
 *
 *      @author
 *              Roger George Doss
 */
#ifndef _PLATFORM_PTRACE_H
#define _PLATFORM_PTRACE_H 1

/* special defines
 */
#define PROC_TRACE_SYSCALL 1

/* C offsets into struct cpu_ctx
 * for various fields.
 * NOTE: The segment selectors are 16bits,
 *	 but are stored in the lower word of 
 *	 32bit field.
 */
#define PTRACE_DS 		0
#define PTRACE_ES 		1
#define PTRACE_FS 		2
#define PTRACE_GS 		3

#define PTRACE_EDI              4
#define PTRACE_ESI              5
#define PTRACE_EBP              6
#define PTRACE_ESP		7
#define PTRACE_EBX              8
#define PTRACE_EDX              9
#define PTRACE_ECX              10
#define PTRACE_EAX 		11	

#define PTRACE_ORIG_EAX 	12
#define PTRACE_EIP 		13
#define PTRACE_CS  		14
#define PTRACE_EFLAGS		15
#define PTRACE_ESP 		16
#define PTRACE_SS   		17

#define PTRACE_GETREGS		1
#define PTRACE_SETREGS		2
#define PTRACE_GETFPREGS	3
#define PTRACE_SETFPREGS	4

#endif /* _PLATFORM_PTRACE_H */
