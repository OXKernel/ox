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
 * Copyright (C) 1999. Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 *	@module  
 *		signal.c 
 *
 * 	@description
 *
 *	    	Contains signal handling code for OX kernel.
 *
 *	@note
 *	        Portions of this code were derived from linux.
 *
 */
#include <ox/linkage.h>
#include <platform/ptrace.h>
#include <platform/cpu_ctx.h>
#include <ox/error_rpt.h>
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

#include <ox/process.h>
#include <ox/scheduler.h>
#include <ox/mm/page.h>
#include <ox/mm/malloc.h>
#include <platform/asm_core/util.h>
#include <ox/fs.h>
#include <ox/kernel.h>
#include <ox/exit.h>

#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

int ksignal(int signal, sighandler_t handler)
{
    sighandler_t oldhandler = NULL;
    if(signal < SIGHUP || signal > SIGTTOU || signal==SIGKILL || !handler) {
        return -1;
    }
    // Restorer is deprecated.
    oldhandler = current_process->p_sigaction[signal].sa_handler;
    current_process->p_sigaction[signal].sa_handler = handler;
    current_process->p_sigaction[signal].sa_flags = 0;
    current_process->p_sigaction[signal].sa_mask = SA_ONESHOT | SA_NOMASK;
    // 64bit issue.
    // Casting pointer to int.
    return (int)oldhandler;

}/* ksignal */

int ksigaction(int signal, const struct sigaction *action,
                                 struct sigaction *old)
{
    if(signal < SIGHUP || signal > SIGTTOU || signal==SIGKILL ||
       !action || !old) {
        return -1;
    }
    // Copy old sigaction.
    *old = current_process->p_sigaction[signal];
    // Setup new sigaction.
    current_process->p_sigaction[signal] = *action;
    // Setup mask.
    if(current_process->p_sigaction[signal].sa_flags & SA_NOMASK) {
        current_process->p_sigaction[signal].sa_mask = 0;
    } else {
        current_process->p_sigaction[signal].sa_mask |= ( 1 << signal );
    }
    return 0;
}/* ksigaction */

/*
 * whichbit:
 * Given an integer 'bits' return
 * the first bit that is on.
 */
int whichbit(unsigned long bits)
{
    int i = 0;
    int maxbit = sizeof(bits) * 8;
    for(i = 0; i < maxbit; ++i) {
        if(bits & i) {
            return i;
        }
    }
    return -1;
}/* whichbit */

/*
 * In syscall.s
 * there is a check of current_process->p_blocked value where
 * the not of this value is logically and with p_signal
 * if the value is not zero (meaning something is not blocked)
 * then the call to syscall_signal_return is made
 * which calls signal_exec with the mask and cpu context.
 *
 * See also:=
 * For offsets
 * ./platform/i386/arch/asm_core/common/proc.inc
 *
 * For struct process:=
 * ./include/ox/process.h
 *
 * This implies that oldmask has a flag for which
 * signal are blocked i.e., contents of ebx from :=
 * mov dword ecx, [__PROC_BLOCKED__ + eax]
 * mov dword ebx,ecx
 * And later on:=
 * push dword ebx
 *
 * Thus, we should check which bit is set in p_signal
 * given the mask which indicates which bits are blocked, 
 * and call the appropriate signal handler.
 *
 */
__clinkage__
int signal_exec(unsigned long oldmask, struct cpu_ctx *ctx)
{
    int signal = whichbit(current_process->p_signal & ~oldmask);
    sighandler_t handler    = NULL;
    struct sigaction *sig   = NULL;
    long eip = 0;
    
    if(signal == 0) {
        return 0;
    }
    if(signal < SIGHUP || signal > SIGTTOU || !ctx) {
        return -1;
    }
    sig = &current_process->p_sigaction[signal];
    eip = ctx->eip;
    handler = (sighandler_t)sig->sa_handler;
    if(handler == SIG_IGN) {
        // Ignore.
        return 0;
    }
    if(handler == SIG_DFL) {
        if(signal == SIGCHLD) {
            return 0;
        } else {
            // Default behavior is to exit.
            kexit(1 << signal);
        }
    }
    if(sig->sa_flags & SA_ONESHOT) {
        // Handle one time.
        sig->sa_handler = NULL;
    }
    // Setup instruction pointer to point
    // to the user supplied signal handler.
    ctx->eip = (long)handler;
    // Place on the stack the signal number
    // this is what sighandler_t expects
    // as an input parameter.
    // TODO - I am not sure how to get it to execute
    //        the signal handler stored in eip above.
    ctx->esp -= 4;
    (*(long *)ctx->esp) = signal;
    ctx->esp += 4;
    // Setup blocked for next run.
    current_process->p_blocked |= sig->sa_mask;
    return 0;
}/* signal_exec */

/*
 * EOF
 */
