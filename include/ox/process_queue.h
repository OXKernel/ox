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
 * <ox/process_queue.h>
 *
 * Kernel private routines for manipulating struct process
 * as a queue, used in scheduling,etc. 
 ********************************************************/
#ifndef _OX_PROCESS_QUEUE_H
#define _OX_PROCESS_QUEUE_H 1
#ifdef __cplusplus
extern "C"{
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define PANIC(message)                                                  \
        panic("line (%d), file (%s)\nERROR (%s)\n",__LINE__,__FILE__,(message));
#else
#define PANIC(message)							\
	panic((message));
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define CHECK_PROC_PRIORITY(proc_q) 					\
	 if ( (proc_q) == NULL )					\
                PANIC("trying to read from empty queue");		\
									\
        if ( (proc_q)->p_priority > Nr_PRIORITY )			\
                PANIC("process priority greator than Nr_PRIORITY");	\
									\
        if ( (proc_q)->p_priority < 0 )					\
                PANIC("process priority less than zero");
#else
#define CHECK_PROC_PRIORITY(proc_q)
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define CHECK_PROC_PTR(proc_q)						\
	if( (proc_q) == NULL )						\
		PANIC("NULL process pointer");
#else
#define CHECK_PROC_PTR(proc_q)
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define CHECK_PRIORITY(priority)					\
	if ( (priority) > Nr_PRIORITY )					\
		PANIC("process priority greator than Nr_PRIORITY");	\
	if ( (priority) < 0 )						\
		PANIC("process priority less than zero");
#else
#define CHECK_PRIORITY(priority)
#endif

#ifdef __VERBOSE_ERROR_CHECKING__
#define CHECK_QUEUE_SIZE(size)						\
	if ( (size) == 0 &&						\
                process_tab[priority] != NULL )				\
                PANIC("queue size is zero, with non empty queue");	\
									\
        if ( (size) != 0 &&						\
              process_tab[priority] == NULL )				\
                PANIC("queue size not zero, with empty queue");
#else
#define CHECK_QUEUE_SIZE(size)
#endif

struct process *
queue_read_front(struct process *proc_q);

void
queue_insert(struct process *proc_q,struct process *proc);

void
queue_insert_p(struct process *proc_q, int priority);

void
queue_remove(struct process *proc_q);

unsigned long
queue_size(struct process *proc_q);

void
queue_increment_size(int priority);

void
queue_decrement_size(int priority);

#ifdef __cplusplus
 }
#endif
#endif /* _OX_PROCESS_QUEUE_H */
