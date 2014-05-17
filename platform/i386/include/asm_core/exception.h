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
 *  <platform/i386/asm_core/exception.h>
 *
 *       machine dependent excpetion routines
 *       prototyped hear in C; however,
 *       these are assembler routines
 *
 *******************************************************/
#ifndef _PLATFORM_I386_EXCEPTION_H
#define _PLATFORM_I386_EXCEPTION_H  1
#ifdef __cplusplus
 extern "C" {
#endif

extern
void	excpt_divide_error ( void );

extern
void excpt_debug ( void );

extern
void excpt_nmi( void );

extern
void excpt_int3( void );

extern
void excpt_overflow( void );

extern
void  excpt_bounds( void );

extern
void  excpt_invalid_operation( void );

extern
void excpt_device_not_available( void );

extern
void excpt_coprocessor_segment_overrun( void );

extern
void excpt_reserved( void );

extern
void excpt_coprocessor_error( void );

extern
void excpt_invalid_TSS( void );

extern
void excpt_segment_not_present( void );

extern
void excpt_stack_segment( void );

extern
void excpt_general_protection( void );

extern
void excpt_alignment_check( void );

extern
void excpt_page_fault( void );

extern
void excpt_double_fault( void );

extern
void excpt_restore_math_state( void );

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_EXCEPTION_H */
