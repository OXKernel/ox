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
 * @module  <platform/i386/asm_core/asm_core.h
 *
 * 	this is the main kernel interface file
 * 	declaring asm scheduling routines	    
 */
#ifndef _PLATFORM_I386_ASM_CORE_H
#define _PLATFORM_I386_ASM_CORE_H 1
#ifdef __cplusplus
extern "C" {
#endif

#include <platform/asm_core/scheduler.h>
#include <platform/asm_core/exception.h>
#include <platform/asm_core/interrupt.h>
#include <platform/asm_core/syscall.h>
#include <platform/asm_core/util.h>
#include <platform/asm_core/atom.h>

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_ASM_CORE_H */
