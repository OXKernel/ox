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
 * <platform/i386/exception.h>
 *
 ********************************************************/
#ifndef _PLATFORM_I386_EXCEPTION_H
#define _PLATFORM_I386_EXCEPTION_H 1
#ifdef __cplusplus
 extern "C" {
#endif

/*	no error codes
 */	
extern
void do_divide_error(struct  cpu_ctx *ctx, int error_code );

extern
void do_debug(struct  cpu_ctx *ctx, int error_code );

extern
void do_nmi(struct  cpu_ctx *ctx, int error_code );

extern
void do_int3(struct  cpu_ctx *ctx, int error_code );

extern
void do_overflow(struct  cpu_ctx *ctx, int error_code );

extern
void do_bounds(struct  cpu_ctx *ctx, int error_code );

extern
void do_invalid_operation(struct  cpu_ctx *ctx, int error_code );

extern
void do_device_not_available(struct  cpu_ctx *ctx, int error_code );

extern
void do_coprocessor_segment_overrun(struct  cpu_ctx *ctx, int error_code );

extern
void do_reserved(struct  cpu_ctx *ctx, int error_code );

extern
void do_coprocessor_error(struct  cpu_ctx *ctx, int error_code );

/*
 *	error code exceptions
 */
extern
void do_invalid_TSS(struct  cpu_ctx *ctx, int error_code );

extern
void do_segment_not_present(struct  cpu_ctx *ctx, int error_code );

extern
void do_stack_segment(struct  cpu_ctx *ctx, int error_code );

extern
void do_general_protection(struct  cpu_ctx *ctx, int error_code );

extern
void do_alignment_check(struct  cpu_ctx *ctx, int error_code );

extern
void do_page_fault(struct  cpu_ctx *ctx, int error_code );

extern
void do_double_fault(struct  cpu_ctx *ctx, int error_code );

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_EXCEPTION_H */
