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
 *	sys_call_1.c
 *      contains system calls with one parameter.
 *
 ********************************************************/
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <sys/unistd.h>
#include <platform/asm_core/io.h>
#include <sys/types.h>
#include <dirent.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <platform/asm_core/io.h>
#include <ox/exit.h>
#include <ox/kernel.h>
#include <sys/utsname.h>
#include <ox/version.h>
#include <ox/process.h>
#include <ox/scheduler.h>
#include <ox/ktime.h>

int sys_acct(const char *file)
{
}/* sys_acct */

unsigned int sys_alarm(unsigned int seconds)
{
    unsigned int old_alarm = current_process->p_alarm;
    current_process->p_alarm = seconds;
    current_process->p_start_time = ktime(0);
    return old_alarm;
}/* sys_alarm */

int sys_brk(void *end_data_segment)
{
}/* sys_brk */

void *sys_sbrk(ptrdiff_t increment)
{
}/* sys_sbrk */

int sys_chdir(const char *path)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kchdir(path);
    asm_enable_interrupt();
    return rtvl;
}/* sys_chdir */

int sys_fchdir(int fd)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kfchdir(fd);
    asm_enable_interrupt();
    return rtvl;
}/* sys_fchdir */

int sys_chroot(const char *path)
{
}/* sys_chroot */

int sys_close(int fd)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kclose(fd);
    asm_enable_interrupt();
    return rtvl;
}/* sys_close */

int sys_dup(int fd)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kdup(fd);
    asm_enable_interrupt();
    return rtvl;
}/* sys_dup */

volatile void sys_exit(int status)
{
    kexit(status);
}/* sys_exit */

pid_t sys_getpgid(pid_t pid)
{
    return kgetpgid(pid);
}/* sys_getpgid */

int sys_nice(int val)
{
    return knice(val);
}/* sys_nice */

int sys_pipe(int filedes[2])
{
}/* sys_pipe */

int sys_raise(int sig)
{
}/* sys_raise */

int sys_rmdir(const char *path)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = krmdir(path);
    asm_enable_interrupt();
    return rtvl;
}/* sys_rmdir */

int sys_setgid (gid_t gid)
{
    return ksetregid(gid,gid);
}/* sys_setgid */

int sys_setuid (uid_t uid)
{
    return ksetreuid(uid,uid);
}/* sys_setuid */

int sys_seteuid(uid_t euid)
{
}/* sys_seteuid */

int sys_setegid(gid_t egid)
{
}/* sys_setegid */

int sys_sleep(unsigned int seconds)
{
}/* sys_sleep */

int sys_sigpending(sigset_t *set)
{
}/* sys_sigpending */

int sys_sigsuspend(const sigset_t *mask)
{
}/* sys_sigsuspend */

int sys_sigreturn   (unsigned long __unused)
{
}/* sys_sigreturn */

int sys_sigemptyset(sigset_t *set)
{
}/* sys_sigemptyset */

int sys_sigfillset(sigset_t *set)
{
}/* sys_sigfillset */

time_t sys_time(time_t *t)
{
    return ktime(t);
}/* sys_time */

clock_t sys_times(struct tms *buf)
{
    return ktimes(buf);
}/* sys_times */

time_t sys_mktime(struct tm *tm)
{
    return kmktime(tm);
}/* sys_mktime */

int sys_ftime(struct timeb *tp)
{
}/* sys_ftime */

int sys_stime(time_t *t)
{
}/* sys_stime */

mode_t sys_umask(mode_t mask)
{
    int old_mask = current_process->umask;
    current_process->umask = mask & 0777;
    return old_mask;
}/* sys_umask */

int sys_umount(const char *specialfile)
{
}/* sys_umount */

int sys_uname(struct utsname *name)
{
    if(!name) {
        return -1;
    }
    strcpy(name->sysname,"OX");
    strcpy(name->nodename,"nodename");
    strcpy(name->release,OX_RELEASE_STR);
    strcpy(name->version,OX_VERSION_STR);
    strcpy(name->machine,"machine");
    strcpy(name->domainname,"dommainname");
    return 0;
}/* sys_uname */

int sys_unlink(const char *path)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kunlink(path);
    asm_enable_interrupt();
    return rtvl;
}/* sys_unlink */

pid_t sys_wait(WAIT_STATUS wait_stat)
{
}/* sys_wait */

char *sys_getwd(char *buf)
{
    char *rtvl = 0;
    asm_disable_interrupt();
    rtvl = kgetwd(buf);
    asm_enable_interrupt();
    return rtvl;
}/* sys_getwd */

DIR *sys_opendir(const char *path)
{
    DIR *rtvl = 0;
    asm_disable_interrupt();
    rtvl = kopendir(path);
    asm_enable_interrupt();
    return rtvl;
}/* sys_opendir */

int sys_closedir(DIR *dir)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kclosedir(dir);
    asm_enable_interrupt();
    return rtvl;
}/* sys_closedir */

void sys_rewinddir(DIR *dir)
{
    asm_disable_interrupt();
    krewinddir(dir);
    asm_enable_interrupt();
}/* sys_rewinddir */

struct dirent *sys_readdir(DIR *dir)
{
    struct dirent *rtvl = 0;
    asm_disable_interrupt();
    rtvl = kreaddir(dir);
    asm_enable_interrupt();
    return rtvl;
}/* sys_readdir */
/*
 * EOF
 */
