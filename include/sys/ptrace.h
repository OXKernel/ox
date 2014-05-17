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
 *  <sys/ptrace.h>
 *   process trace system call
 * 
 *  
 ********************************************************/
#ifndef  _SYS_PTRACE_H
#define  _SYS_PTRACE_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* various requests to ask of ptrace 
 */
#define PTRACE_TRACEME     0 /* enable trace by parent 
                              * for this process 
                              */
#define PTRACE_PEEKTEXT    1 /* return word in process text 
                              * space at address addr 
                              */
#define PTRACE_PEEKDATA    2 /* return word in process 
                              * data space at address addr 
                              */
#define PTRACE_PEEKUSER    3 /* return word in process's user 
                              * area, at offest addr 
                              */
#define PTRACE_POKETEXT    4 /* write word data into process's text 
                              * space at address addr 
                              */ 
#define PTRACE_POKEDATA    5 /* write word data into process's data 
                              * space at address addr 
                              */
#define PTRACE_POKEUSER    6 /* write word data into process's user 
                              * area space at offest addr 
                              */
#define PTRACE_CONT        7 /* continue the process */
#define PTRACE_KILL        8 /* kill the process */

/* The following maynot be available on all machines */
#define PTRACE_SINGLESTEP  9 /* single step the process */
#define PTRACE_GETREGS    12 /* get all general purpose registers used */
#define PTRACE_SETREGS    13 /* set all general purpose registers */
#define PTRACE_GETFPREGS  14 /* get all floating point registers */
#define PTRACE_SETFPREGS  15 /* set all floating point registers */

#define PTRACE_ATTACH     16 /* attach to a process already running */
#define PTRACE_DETACH     17 /* detach from a process */
#define PTRACE_SYSCALL    24 /* continue/stop at next return from syscall */

extern int ptrace (int request,pid_t pid,long addr,long data);
#ifdef __cplusplus
 }
#endif
#endif /* _SYS_PTRACE_H */ 
