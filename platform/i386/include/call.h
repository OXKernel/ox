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
 * <platform/i386/call.h>
 *    UNIX system calls numbers
 *
 ********************************************************/
#ifndef _PLATFORM_I386_CALL_H
#define _PLATFORM_I386_CALL_H  1
#ifdef __cplusplus
 extern "C" {
#endif

#define   __CALL_startup       0
#define   __CALL_access        1
#define   __CALL_acct          2
#define   __CALL_alarm         3
#define   __CALL_brk           4
#define   __CALL_chdir         5
#define   __CALL_chmod         6
#define   __CALL_chown         7
#define   __CALL_chroot        8
#define   __CALL_creat         9
#define   __CALL_close        10
#define   __CALL_dup          11
#define   __CALL_dup2         12
#define   __CALL_execve       13
#define   __CALL_exit         14
#define   __CALL_fcntl        15
#define   __CALL_fork         16
#define   __CALL_getpid       17
#define   __CALL_getegid      18
#define   __CALL_getgid       19
#define   __CALL_geteuid      20
#define   __CALL_getuid       21
#define   __CALL_getppid      22
#define   __CALL_getpgid      23
#define   __CALL_getcwd       24
#define   __CALL_ioctl        25
#define   __CALL_kill         26
#define   __CALL_link         27
#define   __CALL_lseek        28
#define   __CALL_mkdir        29
#define   __CALL_mknod        30
#define   __CALL_mount        31
#define   __CALL_nice         32
#define   __CALL_open         33
#define   __CALL_pause        34
#define   __CALL_pipe         35
#define   __CALL_ptrace       36
#define   __CALL_read         37
#define   __CALL_rmdir        38
#define   __CALL_setpgid      39
#define   __CALL_setgid       40
#define   __CALL_setuid       41
#define   __CALL_setsid       42
#define   __CALL_setreuid     43     
#define   __CALL_setregid     44    
#define   __CALL_sleep        45      /* RGD Is this a system call  */
#define   __CALL_nanosleep    46  

/* signal handlers */
#define   __CALL_sigaction    47 
#define   __CALL_sigprocmask  48
#define   __CALL_sigpending   49 
#define   __CALL_sigsuspend   50
#define   __CALL_sigreturn    51
#define   __CALL_sigemptyset  52
#define   __CALL_sigfillset   53
#define   __CALL_sigaddset    54
#define   __CALL_sigdelset    55
#define   __CALL_sigismember  56

#define   __CALL_stat         57
#define   __CALL_fstat        58 
#define   __CALL_sync         59
                            
/* time */        
#define   __CALL_time         60
#define   __CALL_times        61 
#define   __CALL_ftime        62
#define   __CALL_stime        63
#define   __CALL_utime        64
        
#define   __CALL_umask        65 
#define   __CALL_umount       66
#define   __CALL_uname        67 
#define   __CALL_unlink       68
#define   __CALL_ustat        69
        
#define   __CALL_waitpid      70 
#define   __CALL_write        71 

/* reboot */
#define   __CALL_reboot       72
#define   __CALL_lstat        73
#define   __CALL_fchmod       74
#define   __CALL_getwd        75
#define   __CALL_get_current_dir_name 76
#define   __CALL_fchdir       77
#define   __CALL_rename       78
#define   __CALL_opendir      79
#define   __CALL_closedir     80
#define   __CALL_rewinddir    81
#define   __CALL_readdir      82
#define   __CALL_fchown       83
#define   __CALL_lchown       84
#define   __CALL_signal       85
#define   __CALL_mktime       86

#ifdef __cpluplus
 }
#endif
#endif /* _PLATFORM_I386_CALL_H */
