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
 *	sys_call_2.c
 *      contains system calls with 2 parameters.
 *
 ********************************************************/
// Include unistd, but the actual function definitions
// are for user space. We have internal implementations
// inside the kernel. The user space calls are actually
// assembler hooks setting up parameters in registers
// and calling int 0x80 to enter kernel mode.
// The body of the asm user entry points are in 
// ./libc/syscall/*.c
// Where there is a function for each syscall.
// The entry points into the kernel are implemented
// in this directory, ./kernel/syscall/*.c where
// each system call is organized by the number of
// parameters it has.
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <platform/asm_core/io.h>
#include <platform/asm_core/io.h>
#include <sys/types.h>
#include <dirent.h>
#include <ox/exit.h>
#include <ox/kernel.h>

int sys_access(const char *path,mode_t mode)
{
}/* sys_access */

int sys_chmod(const char *path,mode_t mode)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kchmod(path,mode);
    asm_enable_interrupt();
    return rtvl;
}/* sys_chmod */

int sys_fchmod(int fd, mode_t mode)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kfchmod(fd,mode);
    asm_enable_interrupt();
    return rtvl;
}

int sys_creat(const char *path,mode_t mode)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kcreat(path,mode);
    asm_enable_interrupt();
    return rtvl;
}/* sys_creat */

int sys_dup2(int fd1,int fd2)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kdup2(fd1,fd2);
    asm_enable_interrupt();
    return rtvl;
}/* sys_dup2 */

int sys_fstat(int fd,struct stat *stbuf)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kfstat(fd,stbuf);
    asm_enable_interrupt();
    return rtvl;
}/* sys_fstat */

char *sys_getcwd(char *buf,size_t size)
{
    char *rtvl = 0;
    asm_disable_interrupt();
    rtvl = kgetcwd(buf,size);
    asm_enable_interrupt();
    return rtvl;
}/* sys_getcwd */

int sys_kill(pid_t pid,int sig)
{
    return kkill(pid, sig);
}/* sys_kill */

int sys_link(const char *oldpath,const char *newpath)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = klink(oldpath,newpath);
    asm_enable_interrupt();
    return rtvl;
}/* sys_link */

int sys_mkdir(const char *path,mode_t mode)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kmkdir(path,mode);
    asm_enable_interrupt();
    return rtvl;
}/* sys_mkdir */

int sys_nanosleep(const struct timespec *req,struct timespec *rem)
{
}/* sys_nanosleep */

int sys_setpgid(pid_t pid, pid_t pgid)
{
}/* sys_setpgid */

int sys_setregid(gid_t rgid, gid_t egid)
{
    return ksetregid(rgid,egid);
}/* sys_setregid */

int sys_setreuid(uid_t ruid, uid_t euid)
{
    return ksetreuid(ruid,euid);
}/* sys_setreuid */

int sys_sigaddset(sigset_t *set, int signum)
{
}/* sys_sigaddset */

int sys_sigdelset(sigset_t *set, int signum)
{
}/* sys_sigdelset */

int sys_sigismember(const sigset_t *set, int signum)
{
}/* sys_sigismember */

int sys_stat(const char *path,struct stat *stbuf)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kstat(path,stbuf);
    asm_enable_interrupt();
    return rtvl;
}/* sys_stat */

int sys_ustat(dev_t dev, struct ustat *ubuf)
{
}/* sys_ustat */

int sys_lstat(const char *path, struct stat *buf)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = klstat(path,buf);
    asm_enable_interrupt();
    return rtvl;
}/* sys_lstat */

int sys_utime(const char *file, struct utimbuf *buf)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = kutime(file,buf);
    asm_enable_interrupt();
    return rtvl;
}/* sys_utime */

int sys_rename(const char *oldpath, const char *newpath)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = krename(oldpath,newpath);
    asm_enable_interrupt();
    return rtvl;
}/* sys_utime */

int sys_signal(int signum, sighandler_t handler)
{
    return ksignal(signum, handler);
}/* sys_signal */
/*
 * EOF
 */
