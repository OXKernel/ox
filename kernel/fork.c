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
 * 		fork.c
 *
 *  @notes:
 *      To traverse the doubly linked circular list, we 
 *      can do the following :=
 *
 *  struct process *curr = NULL;
 *  curr = process_tab[0];
 *  do {
 *      printf("value=%d\n",curr->p_value);
 *      curr = curr->p_next;
 *  } while(curr != process_tab[0]);
 *
 *  @todo:
 *      At present, the kernel places all processes into the
 *      same address space, therefore, all segments should
 *      be the same (as kernel) and GDT should be as well.
 *      We also need to correctly setup the eip to 
 *      point to the copy of the process made here.
 *
 *      Portions of this code is derived from linux.
 *
 ********************************************************/
#include <ox/error_rpt.h>
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/protected_mode.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <ox/fork.h>
#include <ox/process.h>
#include <ox/scheduler.h>
#include <ox/mm/page.h>
#include <ox/mm/malloc.h>
#include <platform/segment.h> // For GDT.
#include <platform/protected_mode.h> // For TSS init.
#include <platform/segment_selectors.h> // For KERNEL_DS/KERNEL_CS.
#include <platform/asm_core/util.h>
#include <string.h>

extern void init(void); // From init.s

static
void load_tss_in_gdt(void *tss, unsigned char priv, int gdt)
{
    struct segment_descriptor *psd = &GDT[gdt];
    // If effective is root, then set to priv 0 (system)
    // otherwise, set to user level.
    struct segment_descriptor *tpsd = get_segment_descriptor(gdt);
#ifdef _DEBUG
    printk("priv %d psd %d psd %d line %d file %s\n",priv,psd,tpsd,__LINE__,__FILE__);
#endif
    protect_init_tsssegment(psd,
                           (unsigned int)tss,
                           sizeof(struct tss),
                           priv);
}// load_tss_in_gdt

static
int kgenpid()
{
    static int pid = 2;
    return (pid++ % (1 << 30));
}// kgenpid

void init_proc2_loop()
{
    for(;;) {
        //printk("init_proc2_loop:: idle\n");
        asm_enable_interrupt(); // scheduler() re-enables
    }
}

void init_proc_loop()
{
    static int count = 0;
    if(count == 0) {
        create_init2_task();
        count++;
    }
    for(;;) {
        //printk("init_proc_loop:: idle\n");
        asm_enable_interrupt(); // scheduler() re-enables
    }

}// init_proc_loop

void create_init2_task()
{
    // Create a struct process entry.
    // Duplicate the current_process.
    // Deep copy the current_process memory.
    // Setup the new process return (eax) to be 0.
    // Queue the process into the process queue.
    // Return from this syscall with the child pid.
    unsigned long msize = PAGE_SIZE + sizeof(struct process);
    struct process *proc = (struct process *)kmalloc(msize);
    unsigned int i = 0, j = 0;

    printk("create_init2_task called line %d file %s\n",__LINE__,__FILE__);

    memset(proc, 0x0, msize);
    i = alloc_gdt();
    load_tss_in_gdt(&(proc->p_tss), 0, i);
    proc->p_tss_seg = i * 8; // Offset into GDT table.
    proc->p_state   = P_UNINTERUPTIBLE;
    proc->p_pid     = 1;
    proc->p_parent  = 1;
    proc->p_counter = (1 << 20); // Starting count. Decrement and pick lowest.
    proc->p_priority= 0; // Initially on the first queue.
    proc->p_tss.previous_task_link = 0;
    proc->p_tss.esp0 = msize + (long)proc; // Assume that stack grows down.
    proc->p_tss.ss0 = KERNEL_CS; // 0x10; // This is from linux, not sure why.
    proc->p_tss.cr3 = get_page_dir();
    proc->p_tss.eip = init_proc2_loop;
    // From http://www.acm.uiuc.edu/sigops/roll_your_own/5.a.html
    proc->p_tss.eflags = 0x0002;
    proc->p_tss.eax = 0; // Return value for child is 0.
    proc->p_tss.ebx = 0;
    proc->p_tss.ecx = 0;
    proc->p_tss.edx = 0;
    proc->p_tss.esp = 0;
    proc->p_tss.esp = msize + (long)proc; // RGDDEBUG
    proc->p_tss.ebp = 0;
    proc->p_tss.esi = 0;
    proc->p_tss.edi = 0;
    proc->p_tss.es = KERNEL_DS;
    proc->p_tss.cs = KERNEL_CS;
    proc->p_tss.ss = KERNEL_CS;
    proc->p_tss.ds = KERNEL_DS;
    proc->p_tss.fs = KERNEL_DS;
    proc->p_tss.gs = KERNEL_DS;
    // Save the math cpu state.
    //__asm__("clts; fnsave %0"::"m" (proc->p_tss.i387));
    // According to osdev.net, we can ignore the ldt. See:=
    // http://wiki.osdev.org/GDT_Tutorial#What.27s_so_special_about_the_LDT.3F
    // Its 0 at this point.
    // Copy memory into this process.
    proc->p_exec = NULL;
    proc->p_exec_size = 0;
    proc->p_elf_entry = NULL;
    proc->p_argc = 0;
    proc->p_envp = NULL;
    proc->p_delete_argv = 0;
    proc->p_argv = NULL;
    proc->p_first_exec = 0; // No, we are running in the kernel.

    // Copy files/directories.
    // No files/directories for init process.

    // Setup root access.
    proc->p_uid  = 0;
    proc->p_euid = 0;
    proc->p_suid = 0;
    proc->p_gid  = 0;
    proc->p_egid = 0;
    proc->p_sgid = 0;

    // Insert into queue 0.
    i = proc->p_priority;
    if(process_tab[i]) {
        //printk("line %d file %s\n",__LINE__,__FILE__);
        proc->p_next = process_tab[i]->p_next;
        proc->p_prev = process_tab[i];
        proc->p_next->p_prev = proc;
        process_tab[i]->p_next = proc;
    } else {
        //printk("line %d file %s\n",__LINE__,__FILE__);
        // Insert as the head.
        proc->p_next = proc;
        proc->p_prev = proc;
        process_tab[i] = proc;
    }
    printk("done create_init2_task\n");
}// create_init2_task

void create_init_task()
{
    // Create a struct process entry.
    // Duplicate the current_process.
    // Deep copy the current_process memory.
    // Setup the new process return (eax) to be 0.
    // Queue the process into the process queue.
    // Return from this syscall with the child pid.
    unsigned long msize = PAGE_SIZE + sizeof(struct process);
    struct process *proc = (struct process *)kmalloc(msize);
    unsigned int i = 0, j = 0;

    printk("create_init_task called line %d file %s\n",__LINE__,__FILE__);

    memset(proc, 0x0, msize);
    i = alloc_gdt();
    load_tss_in_gdt(&(proc->p_tss), 0, i);
    proc->p_tss_seg = i * 8; // Offset into GDT table.
    proc->p_state   = P_UNINTERUPTIBLE;
    proc->p_pid     = 1;
    proc->p_parent  = 1;
    proc->p_counter = (1 << 20); // Starting count. Decrement and pick lowest.
    proc->p_priority= 0; // Initially on the first queue.
    proc->p_tss.previous_task_link = 0;
    proc->p_tss.esp0 = msize + (long)proc; // Assume that stack grows down.
    proc->p_tss.ss0 = KERNEL_CS; // 0x10; // This is from linux, not sure why.
    proc->p_tss.cr3 = get_page_dir();
    proc->p_tss.eip = init; // init_proc_loop;
    // From http://www.acm.uiuc.edu/sigops/roll_your_own/5.a.html
    proc->p_tss.eflags = 0x0002;
    proc->p_tss.eax = 0; // Return value for child is 0.
    proc->p_tss.ebx = 0;
    proc->p_tss.ecx = 0;
    proc->p_tss.edx = 0;
    proc->p_tss.esp = 0;
    proc->p_tss.esp = msize + (long)proc; // RGDDEBUG
    proc->p_tss.ebp = 0;
    proc->p_tss.esi = 0;
    proc->p_tss.edi = 0;
    proc->p_tss.es = KERNEL_DS;
    proc->p_tss.cs = KERNEL_CS;
    proc->p_tss.ss = KERNEL_CS;
    proc->p_tss.ds = KERNEL_DS;
    proc->p_tss.fs = KERNEL_DS;
    proc->p_tss.gs = KERNEL_DS;
    // Save the math cpu state.
    //__asm__("clts; fnsave %0"::"m" (proc->p_tss.i387));
    // According to osdev.net, we can ignore the ldt. See:=
    // http://wiki.osdev.org/GDT_Tutorial#What.27s_so_special_about_the_LDT.3F
    // Its 0 at this point.
    // Copy memory into this process.
    proc->p_exec = NULL;
    proc->p_exec_size = 0;
    proc->p_elf_entry = NULL;
    proc->p_argc = 0;
    proc->p_envp = NULL;
    proc->p_delete_argv = 0;
    proc->p_argv = NULL;
    proc->p_first_exec = 0; // No, we are running in the kernel.

    // Copy files/directories.
    // No files/directories for init process.

    // Setup root access.
    proc->p_uid  = 0;
    proc->p_euid = 0;
    proc->p_suid = 0;
    proc->p_gid  = 0;
    proc->p_egid = 0;
    proc->p_sgid = 0;

    // Insert into queue 0.
    i = proc->p_priority;
    if(process_tab[i]) {
        //printk("line %d file %s\n",__LINE__,__FILE__);
        proc->p_next = process_tab[i]->p_next;
        proc->p_prev = process_tab[i];
        proc->p_next->p_prev = proc;
        process_tab[i]->p_next = proc;
    } else {
        //printk("line %d file %s\n",__LINE__,__FILE__);
        // Insert as the head.
        proc->p_next = proc;
        proc->p_prev = proc;
        process_tab[i] = proc;
    }
}// create_init_task

int kfork(struct cpu_ctx ctx)
{
    // Create a struct process entry.
    // Duplicate the current_process.
    // Deep copy the current_process memory.
    // Setup the new process return (eax) to be 0.
    // Queue the process into the process queue.
    // Return from this syscall with the child pid.
    unsigned long msize = PAGE_SIZE + sizeof(struct process);
    struct process *proc = (struct process *)kmalloc(msize);
    unsigned int i = 0, j = 0;
    unsigned char priv = 0;
    if(current_process->p_euid != 0) {
        // A user process, this probably means
        // our memory map needs to change accordingly.
        priv = 3;
    }
    memset(proc, 0x0, msize);
    i = alloc_gdt();
    load_tss_in_gdt(&(proc->p_tss), priv, i);
    proc->p_tss_seg = i * 8; // Offset into GDT table.
    proc->p_state   = P_UNINTERUPTIBLE;
    proc->p_pid     = kgenpid();
    proc->p_parent  = current_process->p_pid;
    proc->p_counter = (1 << 20); // Starting count. Decrement and pick lowest.
    proc->p_priority= 0; // Initially on the first queue.
    proc->p_tss.previous_task_link = 0;
    proc->p_tss.esp0 = msize + (long)proc; // Assume that stack grows down.
    proc->p_tss.ss0 = KERNEL_DS; // 0x10; // This is from linux, not sure why.
    proc->p_tss.cr3 = get_page_dir();
    proc->p_tss.eip = (long)proc->p_exec + 
                        (ctx.eip - (long)current_process->p_exec);
    proc->p_tss.eflags = ctx.eflags;
    proc->p_tss.eax = 0; // Return value for child is 0.
    proc->p_tss.ebx = ctx.ebx;
    proc->p_tss.ecx = ctx.ecx;
    proc->p_tss.edx = ctx.edx;
    proc->p_tss.esp = ctx.esp;
    proc->p_tss.ebp = ctx.ebp;
    proc->p_tss.esi = ctx.esi;
    proc->p_tss.edi = ctx.edi;
    proc->p_tss.es = ctx.es & 0xffff;
    proc->p_tss.cs = ctx.cs & 0xffff;
    proc->p_tss.ss = ctx.ss & 0xffff;
    proc->p_tss.ds = ctx.ds & 0xffff;
    proc->p_tss.fs = ctx.fs & 0xffff;
    proc->p_tss.gs = ctx.gs & 0xffff;
    // Save the math cpu state.
    __asm__("clts; fnsave %0"::"m" (proc->p_tss.i387));
    // According to osdev.net, we can ignore the ldt. See:=
    // http://wiki.osdev.org/GDT_Tutorial#What.27s_so_special_about_the_LDT.3F
    // Its 0 at this point.
    // Copy memory into this process.
    proc->p_exec = (void *)malloc(current_process->p_exec_size);
    memcpy(proc->p_exec, 
           current_process->p_exec, 
           current_process->p_exec_size);
    proc->p_exec_size = current_process->p_exec_size;
    proc->p_elf_entry = current_process->p_elf_entry;
    proc->p_argc = current_process->p_argc;
    proc->p_envp = current_process->p_envp;
    proc->p_delete_argv = proc->p_delete_argv;
    proc->p_first_exec = 0; // No, we executed the image in parent.
    if(proc->p_delete_argv) {
        // Deep copy argv, and free it
        // in child as well to avoid dangling pointers.
        proc->p_argv = (char **)malloc(sizeof(char *) * proc->p_argc);
        for(i = 0; i < proc->p_argv; ++i) {
            proc->p_argv[i] = (char *)malloc(
                    strnlen(current_process->p_argv[i],1024)+1);
            strncpy(proc->p_argv[i],current_process->p_argv[i],1024);
            proc->p_argv[i][strnlen(current_process->p_argv[i],1024)]='\0';
        }
    }
    // Copy files/directories.
    proc->cwd   = current_process->cwd;
    proc->umask = current_process->umask;
    proc->owner = current_process->owner;
    proc->group = current_process->group;
    for(i = 0; i < MAX_DIR; ++i) {
        proc->dir_tab[i] = current_process->dir_tab[i];
    }
    for(i = 0; i < MAX_FILES; ++i) {
        proc->file_tab[i] = current_process->file_tab[i];
    }
    // Find which file_desc entries match to what file_tab
    // entry in current_process, and setup a reference
    // to the same file in the child proc.
    for(i = 0; i < MAX_FILES; ++i) {
           for(j = 0; j < MAX_FILES; ++j) {
                if(current_process->file_desc[j] == 
                        &(current_process->file_tab[j])) {
                    proc->file_desc[i] = &(proc->file_desc[j]);
           }
        }
    }
    proc->p_uid  = current_process->p_uid;
    proc->p_euid = current_process->p_euid;
    proc->p_suid = current_process->p_suid;
    proc->p_gid  = current_process->p_gid;
    proc->p_egid = current_process->p_egid;
    proc->p_sgid = current_process->p_sgid;

    // Insert into queue 0.
    i = proc->p_priority;
    if(process_tab[i]) {
        proc->p_next = process_tab[i]->p_next;
        proc->p_prev = process_tab[i];
        proc->p_next->p_prev = proc;
        process_tab[i]->p_next = proc;
    } else {
        // Insert as the head.
        proc->p_next = proc;
        proc->p_prev = proc;
        process_tab[i] = proc;
    }
    // Return the child pid.
    return proc->p_pid;
}// kfork
