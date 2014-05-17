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
 * <ox/kernel.h>
 *    kernel master header file
 *
 ********************************************************/
#ifndef _OX_KERNEL_H
#define _OX_KERNEL_H  1
#ifdef __cplusplus
 extern "C" {
#endif
#include <ox/ktime.h>
#include <ox/process.h>
#include <ox/scheduler.h>

#define ROOT 0
#define is_super_user() (current_process->p_euid == ROOT)
int whichbit(unsigned long bits);

/* misc system calls implemented in misc.c
 */
int ksetregid(gid_t rgid, gid_t egid);
int ksetreuid(uid_t ruid, uid_t euid);
int kgetpid();
int kgetppid();
int kgetuid();
int kgeteuid();
int kgetgid();
int kgetegid();

/* sched system calls implemented in scheduler.c
 */
int kpause();
int knice(int value);
int kgetpgid(int pid);
struct process *find_process(int pid);
struct process *find_init();

#ifdef __cplusplus
 }
#endif
#endif /* _OX_KERNEL_H */
