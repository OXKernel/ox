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
 *	sys_call_3.c
 *      contains system calls with 3 paramters.
 *
 ********************************************************/
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <sys/unistd.h>
#include <platform/asm_core/io.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <ox/exit.h>

int sys_fcntl(int fd,int request, ...)
{
}/* sys_fcntl */

int sys_ioctl(int fd, int request, ...)
{
}/* sys_ioctl */

int sys_open(const char *path, int flag, int mode)
{
    // libc/open.c determines if we have a mode
    // parameter passed in based on if (flag & O_CREAT)
    // we here should check if mode == 0, if it is,
    // call kopen, otherwise, call kopen2.
    int rtvl = 0;
    asm_disable_interrupt();
    if(mode == 0) {
        rtvl = kopen(path, flag);
    } else {
        rtvl = kopen2(path, flag, mode);
    }
    asm_enable_interrupt();
    return rtvl;
}/* sys_open */

int sys_chown(const char *path,uid_t owner,gid_t group)
{
}/* sys_chown */

int sys_fchown(int fd,uid_t owner,gid_t group)
{
}/* sys_fchown */

int sys_lchown  (const char *path,uid_t owner,gid_t group)
{
}/* sys_lchown */

int sys_execve(const char *path,const char **argv,const char **envp)
{
}/* sys_execve */

off_t sys_lseek(int fd,off_t offset,int whence)
{
    int rtvl = 0;
    asm_disable_interrupt();
    rtvl = klseek(fd,offset,whence);
    asm_enable_interrupt();
    return rtvl;
}/* sys_lseek */

int sys_mknod(const char *file,mode_t mode,dev_t dev)
{
}/* sys_mknod */

int sys_mount(const char *specialfile,const char *dir,int rwflag)
{
}/* sys_mount */

ssize_t sys_read(int fd,void *buf,size_t count)
{
    ssize_t rtvl = 0;
    asm_disable_interrupt();
    rtvl = kread(fd,buf,count);
    asm_enable_interrupt();
    return rtvl;
}/* sys_read */

int sys_reboot(int magic,int magic_too,int flag)
{
}/* sys_reboot */

int sys_sigaction(int signum,const struct sigaction *act,struct sigaction *oldact)
{
}/* sys_sigaction */

int sys_sigprocmask(int how,const sigset_t *set,sigset_t *oldset)
{
}/* sys_sigprocmask */

pid_t sys_waitpid(pid_t pid,WAIT_STATUS wait_stat,int options)
{
    return kwaitpid(pid, wait_stat, options);
}/* sys_waitpid */

ssize_t sys_write(int fd,void *buf,size_t count)
{
    ssize_t rtvl = 0;
    asm_disable_interrupt();
    rtvl = kwrite(fd,buf,count);
    asm_enable_interrupt();
    return rtvl;
}/* sys_write */

/*
 * EOF
 */
