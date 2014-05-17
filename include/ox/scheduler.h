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
 * Copyright Roger George Doss. All Rights Reserved.
 *************************************************************
 *
 * 	<ox/scheduler.h>
 *         
 *          contains declerations for various scheduler related
 *          code, including pointers for special processes
 *************************************************************/
#ifndef _OX_SCHEDULER_H
#define _OX_SCHEDULER_H 1
#ifdef __cplusplus
 extern "C" {
#endif

extern struct process *process_tab[ Nr_PRIORITY ];
extern struct process *current_process;
extern struct process *previous_process;

/*
 * special process
 */
extern struct process *INIT_TASK;

/* special flags
 */
extern unsigned int dma_active;
extern unsigned int which_queue;

#define ENABLE_QUEUE(queue,priority) \
        if ( (priority) < Nr_PRIORITY ) \
                (queue) |= ( 1 << (priority) );

#define DISABLE_QUEUE(queue,priority) \
        if ( (priority) < Nr_PRIORITY ) \
                (queue) &= ( ~(1 << (priority)));

#define FIRST_PROCESS process_tab[ 0 ];
#define LAST_PROCESS  process_tab[ Nr_PRIORITY - 1];

int scheduler_interrupt_handler(int irq);
void schedule(void );

#ifdef __cplusplus
 }
#endif
#endif /* _OX_SCHEDULER_H */
