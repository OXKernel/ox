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
 *********************************************************
 *
 * 	@module
 *		exit.c 
 *
 *  @note:
 *      portions of this code was derived from linux.
 *
 *********************************************************/
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <ox/error_rpt.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <ox/process.h>
#include <ox/scheduler.h>
#include <ox/mm/page.h>
#include <ox/mm/malloc.h>
#include <platform/asm_core/util.h>
#include <ox/kernel.h>
#include <ox/exit.h>

#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

extern unsigned *GDT_MAP;

void free_process(struct process *proc)
{
    int i = 0, j = 0;
    struct process *tmp = current_process;

    if(!proc) {
        return;
    }
    asm_disable_interrupt();
    // Should match exec.c.
    // Free image.
    if(proc->p_exec) {
        mem_unset_read_only(proc->p_exec,
                            proc->p_exec_size);
        free((void *)proc->p_exec);
    }
    // Free exec arguments.
    if(proc->p_delete_argv) {
        for(i = 0; i < proc->p_argc; ++i) {
            free((void *)proc->p_argv[i]);
        }
        free((void *)proc->p_argv);
    }
    // Close all open files.
    current_process = proc;
    for(i = 0; i < MAX_FILES; ++i) {
        for(j = 0; j < MAX_FILES; ++j) {
            if(proc->file_desc[i] == 
               &(proc->file_tab[j])) {
                kclose(i);
            }
        }
    }
    // Close all open directories.
    for(i = 0; i < MAX_DIR; ++i) {
        if(proc->dir_tab[i].__allocation == DEV_BLOCK_SIZE) {
            kclosedir(&(proc->dir_tab[i])); 
        }
    }
    current_process = tmp;
    // Now unlink proc from the queue.
    if(proc->p_priority < 0 || proc->p_priority >= Nr_PRIORITY) {
        printk("free_process:: priority = [%d]\n",proc->p_priority);
        panic("free_process:: invalid priority"); 
    }

    if(process_tab[proc->p_priority] == proc) {
        process_tab[proc->p_priority] = proc->p_next;
    }

    if(proc->p_next) {
        proc->p_next->p_prev = proc->p_prev;
    }

    if(proc->p_prev) {
        proc->p_prev->p_next = proc->p_next;
    }

    // Now free the associated gdt descriptor.
    i = proc->p_tss_seg / 8;
    if(i < 0 || i > Nr_GDT) {
        printk("tss = [%d]\n",i);
        panic("free_process:: invalid tss\n");
    }
    GDT_MAP[i]=0;

    // Now free the proc.
    kfree((void *)proc);
    asm_enable_interrupt();

}// free_process

static
int dispatch_signal(int signal, int is_root, struct process *proc)
{
    if(signal < SIGHUP || signal > SIGTTOU || !proc) {
        return EINVAL;
    }
    if(is_root || 
       current_process->p_euid == proc->p_euid || 
       is_super_user()) {
        proc->p_signal |= (1 << signal);
        return 0;
    }
    return EPERM;
}// dispatch_signal

static
void terminate_session()
{
    // Find all processes with the same
    // session id and send them a signal.
    struct process *curr = NULL;
    int i = 0;
    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(curr->p_session ==
                   current_process->p_session) {
                    curr->p_signal |= (1 << SIGHUP);
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }
}// terminate_session

static
void signal_parent(int pid)
{
    struct process *curr = NULL;
    int i = 0;
    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(curr->p_pid == pid) {
                    curr->p_signal |= (1 << SIGCHLD);
                    return;
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }
    // No parent found.
    printk("signal_parent parent id = [%d] NOT FOUND\n", pid);
    panic("signal_parent:: parent not found!");

}// signal_parent

int kkill(pid_t pid, int signal)
{
    struct process *curr = NULL;
    int i = 0;
    int rtvl = 0, error = 0;
    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        // Iterate for all processes in the system.
        // Handle cases if pid == 0, pid > 0, pid == -1, 
        // and all other cases (last else statement).
        do {
            if(curr) {
                if(pid == 0 && 
                   curr->p_pgrp == current_process->p_pid) {
                    if((error=dispatch_signal(signal,1,curr))) {
                        rtvl = error;
                    }
                } else if(pid > 0 &&
                          curr->p_pid == pid) {
                        if((error=dispatch_signal(signal,0,curr))) {
                            rtvl = error;
                        }
                } else if(pid == -1) {
                        if((error=dispatch_signal(signal,0,curr))) {
                            rtvl = error;
                        }
                } else {
                    if(curr->p_pgrp == -pid) {
                        if((error=dispatch_signal(signal,0,curr))) {
                            rtvl = error;
                        }
                    }
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }
    return rtvl;
}// kkill

int kexit(long exit_code)
{
    struct process *init = find_init();
    struct process *curr = NULL;
    int i = 0;
    if(!init) {
        printk("kexit:: warning could not find init task\n");
        return 0;
    }
    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(curr->p_parent == current_process->p_pid) {
                    curr->p_parent = 1;
                    if(curr->p_state == P_ZOMBIE) {
                        dispatch_signal(SIGCHLD, init, curr);
                    }
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }
    // TODO:= Sessions are not currently implemented.
    if(current_process->p_leader) {
        terminate_session(); 
    }
    current_process->p_exit_code = exit_code;
    signal_parent(current_process->p_parent);
    schedule();
    return -1;
}// kexit

int kwaitpid(pid_t pid, WAIT_STATUS status, int options)
{
    struct process *curr = NULL;
    int i = 0, fixup = 0;

retry:

    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(curr == current_process) {
                        curr = curr->p_next;
                        continue;
                }
                if(curr->p_parent != current_process->p_pid) {
                        curr = curr->p_next;
                        continue;
                }
                if(pid > 0) {
                    if(curr->p_pid != pid) {
                        curr = curr->p_next;
                        continue;
                    }
                } else if (pid == 0) {
                    if(curr->p_pgrp != current_process->p_pgrp) {
                        curr = curr->p_next;
                        continue;
                    }
                } else if (pid != -1) {
                    if(curr->p_pgrp != -pid) {
                        curr = curr->p_next;
                        continue;
                    }
                }
                if(curr->p_state == P_STOPPED) {
                    if(!(options & WUNTRACED)) {
                        curr = curr->p_next;
                        continue;
                    }
                    *status = 0x7F;
                    return curr->p_pid;
                } else if(curr->p_state == P_ZOMBIE) {
                    current_process->p_cutime += curr->p_utime;
                    current_process->p_cstime += curr->p_stime;
                    pid = curr->p_pid;
                    *status = curr->p_exit_code;
                    free_process(curr);  
                    return pid;
                } else {
                    fixup = 1;
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }

    if(fixup) {
        if(options & WNOHANG) {
            return 0;
        }
        current_process->p_state = P_INTERRUPTIBLE;
        schedule();
        current_process->p_signal &= ~(1 << SIGCHLD);
        if(!(current_process->p_signal & (1 << SIGCHLD))) {
            goto retry;
        } else {
            return EINTR;
        }
        return ECHILD;
    }

}// kwaitpid
