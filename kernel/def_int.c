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
/**************************************************************
 * Copyright (C) Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 * @module  
 *
 * 	def_int.c
 *
 * @description
 *
 *	    Deferred Execution and Interrupts code.
 *	    This is a mechanism
 *	    for allowing a driver to defer less critical work
 *	    to a safer time to execute, ie, when interrupts
 *	    are re-enabled, thus allowing an IRQ handler to function
 *	    much faster by letting time consuming work
 *	    execute at a later time.
 *	    
 *	    Further, kernel code can use this interface
 *	    to install and schedule functions to be executed
 *	    at a deferred time.
 *
 *	    This code is executed via the defint_exec
 *	    routine, and is called at the following times
 *	    by the kernel.  ( All standard Operating System
 *	    Theory. )
 *
 *	    1. After handling a system call.
 *	    2. After handling an exception ( trap ).
 *	    3. After handling an interrupt ( interrupts enabled ).
 *	    4. Before scheduling a new process.
 *
 *	    Most Operating Systems have a similar mechanism(s).
 *
 * 	    _SMP
 *
 * 	    [1] defint_exec(), 
 * 	        defint_install_handler(), 
 * 	        defint_uninstall_handler(), 
 * 	        and defint_exec_selected()
 *
 * 	        need to be locked if run in an multi-processor setting.
 *
 * 	    [2] LOCK_INIT needs to be done via atom_set, to initialize.
 *		We may use 8 bits for flags and type, and 16 bits for count,
 *		so we can automically initialize the entire lock.
 *
 */
#include <stdarg.h>

#include <ox/def_int.h>
#include <ox/bool_t.h>
#include <platform/asm_core/atom.h>

/* DEBUGGING MACROS */
#define DEBUG()        printk("line: %d\n",__LINE__)
#define DEBUG_FLAGS(x) printk("flags: %d\n",(x).flags)
#define DEBUG_COUNT(x) printk("count: %d\n",(x).counter)

/* Macros for dealing with IDs. */
#define NOT_EXEC(id) if(((id) <  NR_DEFINT_INT) || ((id) >= NR_DEFINT))
#define IS_EXEC(id)  if(((id) >= NR_DEFINT_INT) && ((id)  < NR_DEFINT))

#define NOT_INT(id) if(((id) < 0) || ((id) >= NR_DEFINT_INT))
#define IS_INT(id)  if(((id) >=0) && ((id) < NR_DEFINT_INT))

#define ADJUST_EXEC_ID(id) id -= NR_DEFINT_INT

/* 
 * INTERRUPT_COUNTER:
 *	
 *	Tracks the nesting level of Interrupts.
 * 	This is needed because OX is reentrant.
 *     
 *      zero     Indicates Interrupts aer not nested.
 *      non-zero Indicates the number of nested Interrupts.
 */
unsigned long INTERRUPT_COUNTER = 0;

/*
 * DEFINT_PENDING:
 * 	
 * 	Tracks if there are any pending routines to execute.
 * 	This is used in the lower level ( asm_core ) of the kernel
 * 	as an optimization.  By checking if DEFINT_PENDING is
 * 	zero, the system can avoid calling the exec routines
 * 	when they are not needed.
 *
 *	NOTE:
 *		With respect to Execution handlers that have a subtype
 *		other than DEFINT_CURR, the DEFINT_PENDING count
 *		is not affected.
 *
 * 	zero	 Indicates no Deferred Interrupts or Execution.
 * 	non-zero Indicates the number of Deferred Interrupts or Executions.
 */ 	
unsigned long DEFINT_PENDING    = 0;

#define SET_PENDING( x ) 					\
{								\
	register unsigned long __flags__ = (x).flags;		\
	if((!(x).type) && ((__flags__ & 3)==3))			\
			DEFINT_PENDING++;			\
}

#define UNSET_PENDING() --DEFINT_PENDING

/*
 * type  :=>  defint_subtype_t
 *
 * flags :=>  Bit: 1 is the "is scheduled" bit
 *            Bit: 2 is the "is in use"    bit
 *
 * counter:=> Used to syncronize users scheduling/unscheduling
 *            a routine.
 *
 * Operations on this lock are done via the
 * atomic_bit_[set|clear] routines ( ../include/ox/atom.h,
 *				     ../arch/i386/asm_core/atom.s )
 */
typedef struct { 

	unsigned long type, flags;
	unsigned long counter;

}defint_lock_t;

/*
 * FLAGS:
 *
 * Integer values for LOGICAL operations:
 *
 * SCHED_BIT is bit 0, turned on => integer 1.
 * USE_BIT   is bit 1, turned on => integer 2.
 *
 */
#define SCHED_BIT 1
#define USE_BIT	  2

/*
 * Physical bits used:
 */
#define PSCHED_BIT 0
#define PUSE_BIT   1

/*
 * LOGICAL Operations on flags.
 */ 
#define SCHEDULED( x ) 	 ((x).flags &   SCHED_BIT)
#define SET_SCHED( x ) 	 ((x).flags |=  SCHED_BIT)
#define UNSET_SCHED( x ) ((x).flags &= ~SCHED_BIT)
/* The following is an optimization.  The system uses Bit 0,1 to indicate
 * if the handler is scheduled and in use.  This is the only time
 * a handler is run.  Rather than check this explicitly using SCHEDULED()
 * and IN_USE() predicates, we simply check if both bits are on, ie,
 * integer value 3.
 */  
#define RUNNABLE( x )	 ((((x).flags & 3) == 3))

#define NOT_IN_USE( x )  (!((x).flags &  USE_BIT))
#define IN_USE( x )      ((x).flags   &  USE_BIT)
#define SET_USE( x )     ((x).flags  |=  USE_BIT)
#define UNSET_USE( x )   ((x).flags  &= ~USE_BIT)

#define IS_CURR( x )     (!((x).type))
#define IS_TYPE( x, y )  ((x).type == (y))

#define LOCK_INIT( x )   (x).flags = (x).type = 0; (x).counter = 1

typedef struct {

	defint_t 	i_entry;
	defint_lock_t	i_lock;

}defint_tab_t;

typedef struct {

	defint_args_t   e_args;
	defint_exec_t	e_entry;
	defint_lock_t	e_lock;

}defint_exec_tab_t;

static defint_tab_t      defint_tab     [ NR_DEFINT_INT ];
static defint_exec_tab_t defint_exec_tab[ NR_DEFINT_EXEC];

void
defint_exec( void )
{
	int 				i;
	bool 				int_run = false;
	register defint_t 		pf;
	register defint_exec_t 		pef;
	register defint_tab_t 		*ptab  = defint_tab;
	register defint_exec_tab_t 	*petab = defint_exec_tab;

	for(i= 0 ; i < NR_DEFINT_INT; i++, ptab++ ) {
		if(RUNNABLE(ptab->i_lock)) {
			if((pf=ptab->i_entry)) {
				UNSET_USE(ptab->i_lock);
				UNSET_PENDING();
				(*pf)();
			}
		}
	}

	for(i =0 ; i < NR_DEFINT_EXEC; i++, petab++ ) {
		if(IS_CURR(petab->e_lock) &&
			RUNNABLE(petab->e_lock)) {
			if((pef=petab->e_entry)) {
				UNSET_USE(petab->e_lock);
				UNSET_PENDING();
				(*pef)(petab->e_args);
			}
		}
	}

}/* defint_exec */

void
defint_exec_selected ( defint_subtype_t type )
{
	int				i;
	register defint_exec_t   	pef;
	register defint_exec_tab_t	*petab = defint_exec_tab;

	for(i =0; i < NR_DEFINT_EXEC; i++, petab++ ) {
		if(IS_TYPE(petab->e_lock,type) &&
			RUNNABLE(petab->e_lock)) {
			if((pef=petab->e_entry)) {
				UNSET_USE(petab->e_lock);
				(*pef)(petab->e_args);
			}
		}
	}

}/* defint_exec_selected */

defint_status_t 
defint_install_handler( defint_type_t   type,
			defint_id_t 	*id,
	                defint_pri_t 	priority,
			... )
{
	unsigned long		i;
	unsigned long 		_id;
	unsigned long		limit;
	defint_status_t 	ret;
	va_list 		args;

	va_start(args,priority);

	if(type != DEFINT_INT && type != DEFINT_EXEC) {
		ret = DEFINT_TINVAL;
		goto RETURN;	
	}
	if((priority < 0) || (priority >= NR_DEFINT_PRIORITY)) {
		ret = DEFINT_PRINVAL;
		goto RETURN;
	}
	if(*id == DEFINT_IDALLOC) {
		/* Assign an id based on priority:
		 *
		 * 0 => [0  -  7] range
		 * 1 => [8  - 15] range
		 * 2 => [16 - 23] range
		 * 3 => [24 - 31] range
		 *
		 * If the slots are full for a priority,
		 * then we lower the priority.
		 * Thus, the priority is only
		 * a hint, not mandatory.
		 *
		 */
		limit = ((type==DEFINT_INT)?NR_DEFINT_INT:NR_DEFINT_EXEC);
		for(_id = DEFINT_IDALLOC, i=priority << 3; i < limit; i++ ) {
			if(!defint_tab[i].i_entry) {
				_id = i;
				break;	
			}
		}
		if(_id == DEFINT_IDALLOC) {
			ret = DEFINT_IDINVAL;
			goto RETURN;
		}
		*id = ((type==DEFINT_INT)?_id:(_id + NR_DEFINT_INT));
	}
	else {
		/* Use the caller specified id. */
		_id = *id;
		if( type == DEFINT_INT )
			NOT_INT(_id) {
				ret = DEFINT_IDINVAL;
				goto RETURN;	
			}
		else {/* DEFINT_EXEC */
			NOT_EXEC(_id) {
				ret = DEFINT_IDINVAL;
				goto RETURN;
			}
			ADJUST_EXEC_ID(_id);
		}
	}

	if(type == DEFINT_INT) {
		if(defint_tab[_id].i_entry) {
			ret = DEFINT_IDINVAL;
			goto RETURN;
		}
		defint_tab[_id].i_entry = va_arg(args, defint_t);
		LOCK_INIT(defint_tab[_id].i_lock);
		defint_tab[_id].i_lock.type = DEFINT_CURR;
		ret = DEFINT_OK;
	}
	else {/* DEFINT_EXEC */
		if(defint_exec_tab[_id].e_entry) {
			ret = DEFINT_IDINVAL;	
			goto RETURN;
		}
		defint_exec_tab[_id].e_entry = va_arg(args, defint_exec_t);
		defint_exec_tab[_id].e_args  = va_arg(args, defint_args_t);
		LOCK_INIT(defint_exec_tab[_id].e_lock);
		defint_exec_tab[_id].e_lock.type = va_arg(args, defint_subtype_t);
		ret = DEFINT_OK;
	}

RETURN:
	va_end(args);
	return(ret);

}/* defint_install_handler */

defint_status_t 
defint_uninstall_handler( defint_id_t id )
{
	static const defint_tab_t       _zero_int;
	static const defint_exec_tab_t  _zero_exec;

	IS_INT(id) {
		defint_tab[id] = _zero_int;	
	}
	IS_EXEC(id) {
		ADJUST_EXEC_ID(id);
		defint_exec_tab[id] = _zero_exec;
	}
	else {
		return (DEFINT_IDINVAL);	
	}
	return (DEFINT_OK);

}/* defint_uninstall_handler */

defint_status_t 
defint_schedule( defint_id_t id )
{
	IS_INT(id) {
		if(atom_dec_test(&(defint_tab[id].i_lock.counter))) {
			SET_SCHED(defint_tab[id].i_lock);
		}
	}
	IS_EXEC(id) {
		ADJUST_EXEC_ID(id);
		if(atom_dec_test(&(defint_exec_tab[id].e_lock.counter))) {
			SET_SCHED(defint_exec_tab[id].e_lock);
		}
	}
	else {
		return (DEFINT_IDINVAL);	
	}
	return (DEFINT_OK);

}/* defint_schedule */

defint_status_t 
defint_unschedule( defint_id_t id )
{
	IS_INT(id) {
		UNSET_SCHED(defint_tab[id].i_lock);
		atom_increment(&(defint_tab[id].i_lock.counter));
	}
	IS_EXEC(id) {
		ADJUST_EXEC_ID(id);
		UNSET_SCHED(defint_exec_tab[id].e_lock);
		atom_increment(&(defint_exec_tab[id].e_lock.counter));
	}
	else {
		return (DEFINT_IDINVAL);	
	}
	return (DEFINT_OK);

}/* defint_unschedule */

defint_status_t 
defint_inuse ( defint_id_t id )
{
	IS_INT(id) {
		atom_bit_set((unsigned long *)&(defint_tab[id].i_lock.flags),PUSE_BIT);
		SET_PENDING(defint_tab[id].i_lock);
	}
	IS_EXEC(id) {
		ADJUST_EXEC_ID(id);
		atom_bit_set((unsigned long *)&(defint_exec_tab[id].e_lock.flags),PUSE_BIT);
		SET_PENDING(defint_exec_tab[id].e_lock);
	}
	else {
		return (DEFINT_IDINVAL);	
	}
	return (DEFINT_OK);

}/* defint_inuse */

/*
 * EOF
 */
