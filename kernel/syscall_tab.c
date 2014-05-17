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
 * 		syscall_tab.c
 *	@description
 *		Contains the definition of the
 *	syscall_dispatch_tab.
 *
 *
 ********************************************************/
#include <ox/types.h>
#include <ox/defs.h>
#include <ox/syscall.h>
#include <ox/syscall_dispatch_tab.h>

/* number of system calls
 * implemented, must equal
 * those noted in include/platform/[machine]/call.h
 * NOTE:
 *   The order in which these routines are
 *   placed in the call table **MUST**
 *   equal the order in which they are numbered.
 */
syscall_pointer_t
syscall_dispatch_tab[Nr_SYS_CALL] = {
   sys_startup      ,  /* used only by init */
   sys_access       , 
   sys_acct         ,
   sys_alarm        ,
   sys_brk          ,
   sys_chdir        ,
   sys_chmod        , 
   sys_chown        ,
   sys_chroot       , 
   sys_creat        , 
   sys_close        , 
   sys_dup          ,  
   sys_dup2         ,
   sys_execve       ,
   sys_exit         ,
   sys_fcntl        ,
   sys_fork         ,
   sys_getpid       ,
   sys_getegid      , 
   sys_getgid       , 
   sys_geteuid      , 
   sys_getuid       , 
   sys_getppid      , 
   sys_getpgid      , 
   sys_getcwd       , 
   sys_ioctl        , 
   sys_kill         , 
   sys_link         , 
   sys_lseek        , 
   sys_mkdir        , 
   sys_mknod        ,
   sys_mount        , 
   sys_nice         , 
   sys_open         ,   
   sys_pause        ,
   sys_pipe         ,
   sys_ptrace       , 
   sys_read         ,
   sys_rmdir        ,
   sys_setpgid      ,
   sys_setgid       ,
   sys_setuid       ,
   sys_setsid       ,
   sys_setreuid     ,     
   sys_setregid     ,    
   sys_sleep        ,
   sys_nanosleep    ,   

/* signal handlers */
   sys_sigaction    ,   
   sys_sigprocmask  ,
   sys_sigpending   ,
   sys_sigsuspend   ,
   sys_sigreturn    ,
   sys_sigemptyset  ,
   sys_sigfillset   ,
   sys_sigaddset    , 
   sys_sigdelset    ,
   sys_sigismember  ,
 
   sys_stat         ,
   sys_fstat        , 
   sys_sync         , 
    
/* time */
   sys_time         ,
   sys_times        ,
   sys_ftime        ,
   sys_stime        ,
   sys_utime        ,

   sys_umask        , 
   sys_umount       ,
   sys_uname        ,
   sys_unlink       ,
   sys_ustat        ,
         
   sys_waitpid      ,
   sys_write        ,
       
/* reboot */
   sys_reboot       ,
   sys_lstat        ,
   sys_fchmod       ,
   sys_getwd        ,
   sys_get_current_dir_name,
   sys_fchdir       ,
   sys_rename       ,
   sys_opendir      ,
   sys_closedir     ,
   sys_rewinddir    ,
   sys_readdir      ,
   sys_fchown       ,
   sys_lchown       ,
   sys_signal       ,
   sys_mktime
};

/*
 * EOF
 */
