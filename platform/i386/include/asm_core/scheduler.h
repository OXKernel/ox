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
 * @module  <platform/i386/asm_core/scheduler.h>
 *
 * 	C interface routines for asm scheduling routines	
 */
#ifndef _PLATFORM_I386_ASM_CORE_SCHEDULER_H
#define _PLATFORM_I386_ASM_CORE_SCHEDULER_H 1
#ifdef __cplusplus
extern "C" {
#endif

extern
void asm_idle( void );

extern
void asm_switch( void );

extern
void asm_qswitch( void );

extern
void asm_soft_switch(struct tss *old_tss, struct tss *new_tss, int mode);

extern
void asm_switch_to_current(void);

extern
void c_switch_to_current(void);

extern
void
c_ltr(unsigned short selector);

extern
unsigned int c_str(void);

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_ASM_CORE_SCHEDULER_H */
