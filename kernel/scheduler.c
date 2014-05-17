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
/*********************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 * 
 *	@module
 * 		scheduler.c
 *
 *********************************************************/

#include <ox/error_rpt.h>
#include <ox/linkage.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <ox/fork.h>
#include <ox/exec.h>
#include <ox/syscall.h>
#include <ox/syscall_dispatch_tab.h>

#include <platform/segment.h>
#include <platform/tss.h>
#include <platform/protected_mode_defs.h>

/* File system includes which are
 * referenced in struct process.
 */
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>

#include <ox/process.h>
#include <ox/scheduler.h>

#include <platform/asm_core/scheduler.h>
#include <platform/asm_core/util.h>
#include <platform/interrupt.h>
#include <drivers/chara/pit.h>

void (*entry_point)();

struct process *process_tab[Nr_PRIORITY];

/* special pointers to struct process
 */
struct process *current_process;
struct process *previous_process;
struct process *INIT_TASK;

/* special flags
 */
unsigned int dma_active = 0;
unsigned int which_queue= 0;

struct process *find_process(int pid);

struct process *find_init()
{
    return find_process(1);
}/* find_init */

struct process *find_process(int pid)
{
    struct process *curr = NULL;
    struct process *proc = NULL;
    int i = 0;
    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(curr->p_pid == pid) {
                    proc = curr;
                    return proc;
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }
    return proc;
}/* find_process */

void
scheduler_init( void )
{
	/* set up first entry into the table
	 * statically, representing the INIT_TASK
	 */
    create_init_task();
	INIT_TASK = process_tab[0];
	current_process = process_tab[0];
    pit_install_handler(PIT_SCHEDULER, scheduler_interrupt_handler);

}/* scheduler_init */

int scheduler_interrupt_handler(int irq)
{
    schedule();
    return IRQ_ENABLE;
}/* schedule_interrupt_handler */

void
schedule(void )
{
    // Traverse each queue, and handle alarm.
    static unsigned first_time = 1;
    static unsigned count = 0;
    static unsigned call_count = 0;
    static unsigned can_block = ~((1 << SIGKILL) | (1 << SIGSTOP));
    struct process *curr = NULL;
    struct process *proc = current_process;
    int i = 0;
    int mode = 0;

    asm_disable_interrupt();

    if(!proc) {
        panic("current_process is null\n");    
    }

    //printk("schedule called line %d file %s count[%d]\n",__LINE__,__FILE__,call_count++);

    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(curr->p_alarm && 
                   (curr->p_alarm + curr->p_start_time) >= ktime(0)) {
                    // Dispatch alarm signal.
                    curr->p_signal |= (1 << SIGALRM);
                    curr->p_alarm = 0;
                }
                if((curr->p_signal & ~(can_block & curr->p_blocked)) &&
                    curr->p_state == P_INTERRUPTIBLE) {
                    curr->p_state = P_RUNNING;
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }

    // Traverse each queue, decrement each p_counter
    // of each task, record which one (RUNNING) has the highest
    // value, and switch to it.
    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                // Decrement and converge
                // on most eligible process to run.
                curr->p_counter--;
                ++count;
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }


    for(i = 0; i < Nr_PRIORITY; ++i) {
        curr = process_tab[i];
        do {
            if(curr) {
                if(proc->p_counter < curr->p_counter) {
                    proc = curr;
                }
                curr = curr->p_next;
            } else {
                break;
            }
        } while(curr != process_tab[i]);
    }

    // Now do the context switch in nasm.
    // NOTE: current_process should start out as init
    // process and should not be null.
    // Also, we did not unlink the selected process, and
    // we probably don't need to do that.
    if(proc == NULL) {
        printk("schedule:: warning scheduling next task is NULL\n");
        // We can get here after an interrupt.
        // If there are no tasks to run, just return.
        return;
    }

    // NOTE: We must decrement the current process otherwise the scheduler
    //       will always select it to run and starve out the other tasks.
    current_process->p_counter--;
    if(first_time || 
            (proc != current_process && current_process->p_tss_seg)) {
        // TODO - Do we need cli call here ?
        //        And when will it be unset ?
        previous_process = current_process;
        current_process = proc;
        if(current_process->p_first_exec == 0 ||
           current_process->p_first_exec == 1) {
            // init, either from kernel task (p_first_exec == 0) or
            // user task (p_first_exec == 1)
            mode = 1;
        } else if(current_process->p_first_exec == 2) {
            // running (p_first_exec == 2)
            mode = 0;
        }
        if(current_process->p_first_exec == 1) {
            // Call into the user process with
            // argc, argv, envp using kernel start method.
            current_process->p_tss.eip = &kstart;
        }
        // Set to running.
        current_process->p_first_exec = 2;
        if(current_process->p_tss.eip == 0x0) {
            // This is a kernel panic.
            printk("eip [%d]\n",current_process->p_tss.eip);
            printk("current_process [%d]\n",current_process);
            count = 0;
            for(i = 0; i < Nr_PRIORITY; ++i) {
                curr = process_tab[i];
                do {
                    if(curr) {
                        printk("count [%d] curr [%d]\n",count,curr);
                        ++count;
                        curr = curr->p_next;
                    } else {
                        break;
                    }
                } while(curr != process_tab[i]);
            }
            panic("error current_process->p_tss.eip == 0x0\n");
            return;
        }

        if(first_time) {
            first_time = 0;
            // - Not calling this wont start the second task.
            // - So the scheduler will always have one task.
            asm_soft_switch(0,&(current_process->p_tss),mode);
        } else {
            asm_soft_switch(&(previous_process->p_tss),&(current_process->p_tss),mode);
        }

    }

}// schedule

// c_ltr, c_str, c_switch_to_current are
// from http://www.acm.uiuc.edu/sigops/roll_your_own/5.a.html
void
c_ltr(unsigned short selector)
{
   asm ("ltr %0": :"r" (selector));
}

unsigned int c_str(void)
{
    unsigned int selector;

   asm ("str %0":"=r" (selector));
   return selector;
}

void
c_switch_to_current(void)
{
    unsigned int sel[2] = {0};

    if(current_process) {
        // sel[0] = offset which is not used in a task switch.
        sel[1] = current_process->p_tss_seg;
        printk("c_switch_to_current:: switching task seg %d\n",sel[1]);
        asm("lcall %0": :"m" (*sel));
    } else {
        panic("c_switch_to_current:: null current_process\n");
    }

}// c_switch_to_current

int kpause()
{
    current_process->p_state = P_INTERRUPTIBLE;
    schedule();
    return 0;
}// kpause

int knice(int value)
{
    if((current_process->p_counter - value) > 0) {
        current_process->p_counter -= value;
    }
    return 0;
}// knice

int kgetpgid(int pid)
{
    struct process *proc = NULL;
    if(pid == 0) {
        return current_process->p_gid;
    } else {
        proc = find_process(pid);
        if(proc) {
            return proc->p_gid;
        }
    }
    return -1;
}// kgetpgid

/*
 * EOF
 */
