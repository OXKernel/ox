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
 * <ox/syscall.h>
 *
 *  syscall.h contains extern syscall routines within
 *  the kernel.
 *  
 *    all routines are named sys_NAME(...)
 *
 *  eg, write == sys_write(). The routines default
 *  to function pointers within a function call table
 *  they are referenced by there call number, after
 *  the approperiate parameters have been verified, and
 *  copied from user space to kernel space.
 *
 ********************************************************/
#ifndef _OX_SYSCALL_H
#define _OX_SYSCALL_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* pointer to system call */
typedef int (*syscall_pointer_t)();

extern int  sys_startup();  /* used only by init */
extern int  sys_access ();
extern int  sys_acct   ();
extern int  sys_alarm  ();
extern int  sys_brk    ();
extern int  sys_chdir  ();
extern int  sys_fchdir ();
extern int  sys_chmod  ();
extern int  sys_fchmod ();
extern int  sys_chown  ();
extern int  sys_fchown ();
extern int  sys_lchown ();
extern int  sys_chroot ();
extern int  sys_creat  ();
extern int  sys_close  ();
extern int  sys_dup    ();
extern int  sys_dup2   ();
extern int  sys_execve ();
extern int  sys_exit   ();
extern int  sys_fcntl  ();
extern int  sys_fork   ();
extern int  sys_getpid ();
extern int  sys_getegid();
extern int  sys_getgid ();
extern int  sys_geteuid();
extern int  sys_getuid ();
extern int  sys_getppid();
extern int  sys_getpgid();
extern int  sys_getcwd ();
extern int  sys_getwd  ();
extern int  sys_get_current_dir_name();
extern int  sys_ioctl  ();
extern int  sys_kill   ();
extern int  sys_link   ();
extern int  sys_lseek  ();
extern int  sys_mkdir  ();
extern int  sys_mknod  ();
extern int  sys_mount  ();
extern int  sys_nice   ();
extern int  sys_open   ();
extern int  sys_pause  ();
extern int  sys_pipe   ();
extern int  sys_ptrace ();
extern int  sys_read   ();
extern int  sys_opendir();
extern int  sys_closedir();
extern int  sys_rewinddir();
extern int  sys_readdir();
extern int  sys_rmdir  ();
extern int  sys_rename ();
extern int  sys_setpgid();
extern int  sys_setgid ();
extern int  sys_setuid ();
extern int  sys_setsid ();
extern int  sys_setreuid    ();
extern int  sys_setregid    ();
extern int  sys_sleep       ();  /* RGD Is this a system call  */
extern int  sys_nanosleep   ();

/* signal handlers */
extern int  sys_sigaction   ();
extern int  sys_sigprocmask ();
extern int  sys_sigpending  ();
extern int  sys_sigsuspend  ();
extern int  sys_sigreturn   ();
extern int  sys_sigemptyset ();
extern int  sys_sigfillset  ();
extern int  sys_sigaddset   ();
extern int  sys_sigdelset   ();
extern int  sys_sigismember ();
extern int  sys_signal();

extern int  sys_stat  ();
extern int  sys_fstat ();
extern int  sys_lstat ();
extern int  sys_sync  ();

/* time */
extern int  sys_time  ();
extern int  sys_times ();
extern int  sys_ftime ();
extern int  sys_stime ();
extern int  sys_utime ();

extern int  sys_umask ();
extern int  sys_umount();
extern int  sys_uname ();
extern int  sys_unlink();
extern int  sys_ustat ();

extern int  sys_waitpid ();
extern int  sys_write   ();

/* reboot */
extern int  sys_reboot();
extern int  sys_mktime();

#ifdef __cplusplus
 }
#endif
#endif /* _OX_SYSCALL_H */
