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
 *      @module
 *              process_queue.c
 *
 ********************************************************/

#include <ox/types.h>
#include <ox/defs.h>
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

#include <ox/fork.h>
#include <ox/process.h>
#include <ox/process_queue.h>
#include <ox/scheduler.h>
#include <ox/error_rpt.h>

/* declared static for local linkage
 */
unsigned long process_size_tab[Nr_PRIORITY];

/* these will be in a different file in the
 * real kernel
 */
struct process *
queue_read_front(struct process *proc_q)
{
	int priority = 0;
	long size    = 0;

	CHECK_PROC_PRIORITY(proc_q);

	priority = proc_q->p_priority;
	size     = *(process_size_tab + priority);

	if ( proc_q->p_next == proc_q &&
	     proc_q->p_prev == proc_q && 
	     size == 1 ) {
		/* if this is the only process on the queue
        	 */
		proc_q->p_next = proc_q->p_prev =
		*(process_tab + priority) = NULL;
		process_size_tab[priority]--;

		return ( proc_q );
	}
	else if (  proc_q->p_next == proc_q &&
	           proc_q->p_prev == proc_q && 
            	   size != 1 )
		   PANIC("queue size not equal to one, with a single node queue");

	/* normal queue processing
	 */
	process_tab[priority]->p_prev->p_next = 
	process_tab[priority]->p_next;
	process_tab[priority]->p_next->p_prev =
	process_tab[priority]->p_prev;
	
	process_tab[priority]= process_tab[priority]->p_next;

	proc_q->p_next = proc_q->p_prev = NULL;
	process_size_tab[priority]--;

	return ( proc_q );
}

void
queue_insert(struct process *proc_q,struct process *proc)
{
	int priority = 0;
	CHECK_PROC_PRIORITY(proc_q);
	
	/* assumes that proc_q is an existing queue
	 * NOTE: We never insert anywhere except
	 *	 at the tail of the queue.
	 */
	priority = proc_q->p_priority;
	proc->p_priority = priority;

        proc->p_next = process_tab[priority];           /* point to the head                */
        proc->p_prev = process_tab[priority]->p_prev;   /* point to the former tail         */
        process_tab[priority]->p_prev->p_next = proc;   /* former tail  points to new tail  */
        process_tab[priority]->p_prev = proc;           /* head prev points to new tail     */

        process_size_tab[priority]++;
}

void
queue_insert_p(struct process *proc_q, int priority)
{

	CHECK_PROC_PTR(proc_q);
	CHECK_PRIORITY(priority);

	proc_q->p_priority = priority;
	
	/* general use process queue insertion routine
	 * NOTE: We never insert anywhere except
         *       at the tail of the queue.
	 */
	if ( *(process_tab + priority) == NULL &&
	     *(process_size_tab + priority) == 0 ) {
		/* first time inserting to this queue
		 */
		proc_q->p_next = proc_q->p_prev = proc_q;
		*(process_tab + priority) = proc_q;
		process_size_tab[priority]++;
		return;
	}
	else if ( *(process_tab + priority) == NULL &&
	       *(process_size_tab + priority) != 0 )
		PANIC("NULL process table entry");
	
	/* normal queue processing
	 */
	proc_q->p_next = process_tab[priority];		/* point to the head                */
	proc_q->p_prev = process_tab[priority]->p_prev;	/* point to the former tail         */
	process_tab[priority]->p_prev->p_next = proc_q; /* former tail  points to new tail  */
	process_tab[priority]->p_prev = proc_q;         /* head prev points to new tail     */

	process_size_tab[priority]++;
}

void
queue_remove(struct process *proc_q)
{
	/* remove a process from anywhere in the queue
	 */
	int priority = 0;
	long size    = 0;
	struct process *tmp = NULL;

	CHECK_PROC_PRIORITY(proc_q);

	priority = proc_q->p_priority;
	size     = *(process_size_tab + priority);

	if (  size == 0 &&
	      process_tab[priority] == NULL )
		return;

	CHECK_QUEUE_SIZE(size);

	if (  proc_q->p_next == proc_q &&
	      proc_q->p_prev == proc_q &&
	      size == 1 ) {
		/* if this is the only process on the queue
        	 */
		proc_q->p_next = proc_q->p_prev =
		*(process_tab + priority) = NULL;
		process_size_tab[priority]--;
		return;
	}
	else if (  proc_q->p_next == proc_q &&
	           proc_q->p_prev == proc_q && 
            	   size != 1 )
		   PANIC("queue size not equal to one, with a single node queue");

	/* check if this is the head or tail
	 */
	tmp = *(process_tab + priority);
	if(tmp == proc_q )
		process_tab[priority] = proc_q->p_next;
	if(tmp->p_prev  == proc_q )
		process_tab[priority]->p_prev = proc_q->p_prev->p_prev;
		
		
	proc_q->p_prev->p_next = proc_q->p_next;
	proc_q->p_next->p_prev = proc_q->p_prev;
	proc_q->p_next = proc_q->p_prev = NULL;
	process_size_tab[priority]--;
}

unsigned long
queue_size(struct process *proc_q)
{
	CHECK_PROC_PRIORITY(proc_q);
	return ( process_size_tab[proc_q->p_priority] );
}

void
queue_increment_size(int priority)
{
	CHECK_PRIORITY(priority);
	process_size_tab[priority]++;
}

void
queue_decrement_size(int priority)
{
	CHECK_PRIORITY(priority);
	process_size_tab[priority]--;
}

/*
 * EOF
 */
