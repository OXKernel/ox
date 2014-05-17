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
/*************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *************************************************************
 * 
 * 	<ox/process.h>
 *  
 *          contains the definition of a process structure
 *************************************************************/
#ifndef _OX_PROCESS_H
#define _OX_PROCESS_H 1
#ifdef __cplusplus
 extern "C" {
#endif

#include <signal.h>

enum process_state {

     P_RUNNING		    = 1,
     P_INTERRUPTIBLE    = 2,
     P_UNINTERUPTIBLE   = 3,
     P_ZOMBIE		    = 4,
     P_STOPPED		    = 5

};

typedef int (*p_sighandler_t)();

struct process {

	/* These fields have corresponding
	 * offsets in asm_core/common/proc.inc
	 * for assembly code.
	 */
    long		p_state;
	long		p_counter;
	long		p_priority;
	long		p_signal;

	long		p_blocked;
	long		p_reschedule;
	long		p_flags;
	long		p_dbgreg6;

	long		p_dbgreg7;
	unsigned short  p_tss_seg; // jump to this in asm_switch_to_current.
	int		p_exit_code;

	/* memory
	 */	
	unsigned long	p_end_code;
	unsigned long	p_end_data;
	unsigned long	p_brk;
	unsigned long	p_start_stack;

	long 		p_pid;
	long		p_parent;
	long		p_pgrp;
	long		p_session;
	long		p_leader;

	unsigned short	p_uid;
	unsigned short	p_euid;
	unsigned short	p_suid;
	unsigned short	p_gid;
	unsigned short	p_egid;
	unsigned short	p_sgid;

	long		p_alarm;
	long		p_utime;
	long		p_stime;
	long		p_cutime;
	long		p_cstime;
	long		p_start_time;
   
	unsigned short	p_used_math;

	/* file system
	 */
    int             p_tty;
    // TODO: p_close_on_exec flag should probably be inside
    // the inode_t structure and checked when we do exec
    // to see if we really have to close the file.
    unsigned long   p_close_on_exec;
    block_t         cwd;
    inode_perm_t    umask;
    inode_own_t     owner;
    inode_group_t   group;
    inode_t file_tab[MAX_FILES];
    inode_t *file_desc[MAX_FILES];
    DIR dir_tab[MAX_DIR];

    /* exec
     */
    void   *p_exec;           // Exe image in memory.
    unsigned int p_exec_size; // Size of exe in memory after load.
    void   *p_elf_entry;      // ELF entry point.
    int     p_argc;           // Argument count.
    char  **p_argv;           // Argument pointer.
    char  **p_envp;           // Environment pointer.
    int     p_delete_argv;    // Should we delete argv.
    int     p_first_exec;     // First time executing image.

	/* LDT
	 */
    struct segment_descriptor p_ldt[Nr_PROC_LDT];

	/* signals
	    p_sighandler_t p_sig_restorer;
	    p_sighandler_t p_sig_fn[32];
    */
    struct sigaction p_sigaction[32];

	/* TSS
	 */
	struct tss      p_tss;

	struct process *p_next;
	struct process *p_prev;
};

#ifdef __cplusplus
 }
#endif
#endif /* _OX_PROCESS_H */
