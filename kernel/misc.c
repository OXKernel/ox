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
 *	@module
 * 		misc.c
 *
 *  @notes:
 *      Provides misc system calls mostly related
 *      to processes.
 *
 ********************************************************/
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
#include <ox/fork.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <ox/error_rpt.h>
#include <ox/process.h>
#include <ox/scheduler.h>
#include <ox/mm/page.h>
#include <ox/mm/malloc.h>
#include <platform/asm_core/util.h>
#include <ox/kernel.h>
#include <ox/exit.h>

#include <errno.h>
#include <sys/wait.h>

int ksetregid(gid_t rgid, gid_t egid)
{
    if(rgid > 0) {
        if((current_process->p_gid == rgid) ||
           is_super_user()) {
            current_process->p_gid = rgid;
        } else {
            return EPERM;
        }
    }
    if(egid > 0) {
        if((current_process->p_gid  == egid) ||
           (current_process->p_egid == egid) ||
           (current_process->p_sgid == egid) ||
           is_super_user()) {
            current_process->p_egid = egid;
        } else {
            return EPERM;
        }
    }
    return 0;
}// ksetregid

int ksetreuid(uid_t ruid, uid_t euid)
{
    int old = current_process->p_uid;
    if(ruid > 0) {
        if((current_process->p_euid == ruid) ||
           (old == ruid) ||
           is_super_user()) {
            current_process->p_uid = ruid;
        } else {
            return EPERM;
        }
    }
    if(euid > 0) {
        if((old == euid) ||
           (current_process->p_euid == euid) ||
           is_super_user()) {
            current_process->p_euid = euid;
        } else {
            current_process->p_uid = old;
        }
    }
    return 0;
}// ksetreuid

int kgetpid()
{
    return current_process->p_pid;
}// kgetpid

int kgetppid()
{
    return current_process->p_parent;
}// kgetppid

int kgetuid()
{
    return current_process->p_uid;
}// kgetuid

int kgeteuid()
{
    return current_process->p_euid;
}// kgeteuid

int kgetgid()
{
    return current_process->p_gid;
}// kgetgid

int kgetegid()
{
    return current_process->p_egid;
}// kgetegid

volatile int local_sleep(int seconds)
{
    int i = 0, j = 0; 
    volatile int k = 0;
    for(i = 0; i < seconds; ++i)
        for(j = 0; j < 0xFFFFFF; j++)
             /* wait */
             k++;
    return k;
}/* local_sleep */
/*
 * EOF
 */
