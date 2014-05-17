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
 *  @module   
 *	sys_call_0.c
 *      contains system calls with no paramters.
 *
 ********************************************************/
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <sys/unistd.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <platform/asm_core/io.h>
#include <platform/cpu_ctx.h>
#include <ox/fork.h>
#include <ox/kernel.h>

pid_t sys_fork(unsigned ebx,  // Note it works in reverse order
               unsigned ecx,  // push ecx, push ebx, is ebx followed by ecx
               unsigned edx,
               unsigned esi,
               unsigned edi,
               unsigned ebp,
               struct cpu_ctx ctx)
{
    // The low-level assembler copies the CPU context
    // onto the stack and sets up the contents of the 
    // six registers we are using for system call paramters on the
    // stack. The low-level assembler is written to always
    // do this, disregarding the actual number of parameters
    // the C user space code uses. This is done so that
    // the low-level assembler is common for all system calls.
    // Here; however, we actually need all of the context
    // and therefore this entry point is different so that
    // the higher level kernel C code has access to the CPU context.
    return kfork(ctx);
}/* sys_fork */

pid_t  sys_getpid(void)
{
    return kgetpid();
}/* sys_getpid */

gid_t  sys_getegid(void)
{
    return kgetegid();
}/* sys_getegid */

gid_t  sys_getgid (void)
{
    return kgetgid();
}/* sys_getgid */

uid_t  sys_geteuid(void)
{
    return kgeteuid();
}/* sys_geteuid */

uid_t  sys_getuid(void)
{
    return kgetuid();
}/* sys_getuid */

pid_t  sys_getpgrp(void)
{
}/* sys_getpgrp */

pid_t  sys_getppid(void)
{
    return kgetppid();
}/* sys_getppid */

int sys_pause  (void) 
{
    return kpause();
}/* sys_pause */

int sys_setpgrp(void)
{
}/* sys_setpgrp */

pid_t sys_setsid(void)
{
}/* sys_setsid */

int sys_startup(void)
{/* used only by init */

}/* sys_startup */

int sys_sync(void)
{
    int rtvl = 0;
    asm_disable_interrupt();
    ksync();
    asm_enable_interrupt();
    return rtvl;
}/* sys_sync */

char *sys_get_current_dir_name(void)
{
    char *rtvl = 0;
    asm_disable_interrupt();
    rtvl = kget_current_dir_name();
    asm_enable_interrupt();
    return rtvl;
}/* sys_get_current_dir_name */
/*
 * EOF
 */
