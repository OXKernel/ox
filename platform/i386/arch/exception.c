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
/*************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *************************************************************
 * 
 *	@module  
 *		exception.c
 *
 *	@description
 *          Contains code for handling software exceptions.
 */
#define _POSIX_SOURCE 1
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>

#include <ox/linkage.h>
#include <ox/exit.h>
#include <platform/ptrace.h>
#include <platform/cpu_ctx.h>
#include <ox/error_rpt.h>
#include <ox/mm/malloc.h>

void log_and_exit(const char *mesg, 
                  struct cpu_ctx *ctx, 
                  int error_code)
{
    printk("ERROR=[%s] CODE=[%d]\n",mesg,error_code);
    printk("ds=%d\tes=%d\tfs=%d\tgs=%d\t\n"
           "edi=%d\tesi=%d\tebp=%d\tesp=%d\t\n"
           "ebx=%d\tedx=%d\tecx=%d\teax=%d\t\n"
           "tmp=%d\teip=%d\tcs=%d\teflags=%d\t\n"
           "sys_esp=%d\tss=%d\t\n",
           ctx->ds,ctx->es,ctx->fs,ctx->gs,
           ctx->edi,ctx->esi,ctx->ebp,ctx->esp,
           ctx->ebx,ctx->edx,ctx->ecx,ctx->eax,
           ctx->tmp,ctx->eip,ctx->cs,ctx->eflags,
           ctx->sys_esp,ctx->ss);
    kexit(error_code);
}/* log_and_exit */

/*
 *	no error code exceptions
 */
__clinkage__
void do_divide_error(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("divide_error",ctx,error_code);
}/* do_divide_error */

__clinkage__
void do_debug(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("debug",ctx,error_code);
}/* do_debug */

__clinkage__
void do_nmi(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("nmi",ctx,error_code);
}/* do_nmi */

__clinkage__
void do_int3(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("int3",ctx,error_code);
}/* do_int3 */

__clinkage__
void do_overflow(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("overflow",ctx,error_code);
}/* do_overflow */

__clinkage__
void do_bounds(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("bounds",ctx,error_code);
}/* do_bounds */

__clinkage__
void do_invalid_operation(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("invalid_operation",ctx,error_code);
}/* do_invalid_operation */

__clinkage__
void do_device_not_available(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("device_not_available",ctx,error_code);
}/* do_device_not_available */

__clinkage__
void do_coprocessor_segment_overrun(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("coprocessor_segment_overrun",ctx,error_code);
}/* do_coprocessor_segment_overrun */

__clinkage__
void do_reserved(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("reserved",ctx,error_code);
}/* do_reserved */

__clinkage__
void do_coprocessor_error(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("coprocessor_error",ctx,error_code);
}/* do_coprocessor */

/*
 *	error code exceptions
 */
__clinkage__
void do_invalid_TSS(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("invalid_TSS",ctx,error_code);
}/* do_invalid_TSS */

__clinkage__
void do_segment_not_present(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("segment_not_present",ctx,error_code);
}/* do_segment_not_present */

__clinkage__
void do_stack_segment(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("stack_segment",ctx,error_code);
}/* do_stack_segment */

__clinkage__
void do_general_protection(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("general_protection",ctx,error_code);
}/* do_general_protection */

__clinkage__
void do_alignment_check(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("alignment_check",ctx,error_code);
}/* do_alignment_check */

__clinkage__
void do_page_fault(struct  cpu_ctx *ctx, int error_code )
{
    //setup_paging();
    log_and_exit("page_fault",ctx,error_code);
}/* do_page_fault */

__clinkage__
void do_double_fault(struct  cpu_ctx *ctx, int error_code )
{
    log_and_exit("double_fault",ctx,error_code);
}/* do_double_fault */

/*
 * EOF
 */
