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
 * @module:
 *
 * 	<ox/defint.h>
 *
 * @description:
 *
 * 	This is the interface to the
 * 	Deferred Execution and Interrupts ( DEFINT )
 * 	mechanism.
 *
 * 	The intention is to provide a relatively fast
 * 	mechanism that allows for functions to be run
 *	at a later time.  The functions can be done
 *	on behalf of any kernel code, or explicitly
 *	for an Interrupt handler.
 *
 * 	When they are run for Interrupt handlers,
 * 	the functions run atomically.
 *
 * 	Deferred Execution handlers run after
 * 	Interrupt handlers.
 *
 * 	The mechanism allows for handlers to be
 *  	run at any specified time using a type
 * 	field.  This allows, for example, all
 * 	timer related functions to be run when
 * 	the timer interrupt occurs.
 *
 * 	The system will currently not uninstall
 *	any handlers after they are run.
 *	This is left up to the caller, but maybe
 *	added at a later time.  It can be done
 *	from the handler by passing a structure
 * 	containing the id of the handler as a paramter
 * 	to the handler, thereby allowing
 *	the handler to uninstall itself when done.
 *
 *      This maybe too much work in practice; we could
 *      uninstall Deferred Execution handlers after
 *      they are run.  All this depends on how frequent
 *      a handler is to be called, and whether or not
 *      we have enough slots.  It may be easier to just
 *      us the in_use routine to turn on/off.
 *
 * @author:
 *
 * 	Roger G. Doss
 *
 *******************************************************/
#ifndef _OX_DEFINT_H
#define _OX_DEFINT_H

/* The following parameters are configurable:
 *
 * NR_DEFINT_INT	=> Total number of Deferred Interrupt Handlers.
 * NR_DEFINT_EXEC	=> Total number of Deferred Execution Handlers.
 * NR_DEFINT_PRIORITY	=> Total number of Priority Levels for each type of handler.
 *
 * NOTE: For automatic id allocation, the system uses
 *       the priority as a recommendation as to where
 *       in the table to place the handler.  Thus, it
 *       is important that the following occur:
 *
 *       For NR_DEFINT_INT and NR_DEFINT_EXEC, the values
 *       be divisible by NR_DEFINT_PRIORITY.
 *
 *       This number is used to calculate a starting
 *       index using the following:
 *
 *       i = priority << 3;
 *
 *       Which is equivalent to:
 *       
 *       i = [0-3] * (8=NR_DEFINT_INT/NR_DEFINT_PRIORITY)
 *
 * 	 Therefore, changing these values should be contingent
 * 	 on changing the step in the implementation file.
 * 	 Look at the code in defint_install_handler().
 *
 */
#define NR_DEFINT_INT		32
#define NR_DEFINT_EXEC		32
#define NR_DEFINT_PRIORITY	 4

#define NR_DEFINT 		(NR_DEFINT_INT + NR_DEFINT_EXEC)
#define DEFINT_IDALLOC		-1

extern unsigned long INTERRUPT_COUNTER;
extern unsigned long DEFINT_PENDING;

typedef enum defint_status {

	DEFINT_OK	= 0,	/* Generic OK. 			*/
	DEFINT_FAILED	=-1,	/* Generic FAILED. 		*/
	DEFINT_IDINVAL	=-2,	/* Could not allocate ID. 	*/
	DEFINT_PRINVAL	=-3,	/* Priority value out of range. */
	DEFINT_TINVAL	=-4,	/* Type is not valid.		*/

}defint_status_t;

typedef enum defint_pri {

	DEFINT_PRI_00,
	DEFINT_PRI_01,
	DEFINT_PRI_02,
	DEFINT_PRI_03

}defint_pri_t;

typedef enum defint_type {
	
	DEFINT_EXEC,
	DEFINT_INT

}defint_type_t;

typedef enum defint_subtype {

	DEFINT_CURR,
	DEFINT_TIME,
	DEFINT_DISK,
	DEFINT_SCHED

}defint_subtype_t;

typedef void *defint_args_t;
typedef void (*defint_t)     ( void  );
typedef void (*defint_exec_t)( void *);

/*
 * defint_id_t:
 *
 * 	The minimum available will be
 * 	[0 - ( NR_DEFINT - 1 )].
 *
 * 	The id's are contiguous, with
 *
 * 	[0-NR_DEFINT_INT - 1] => Deferred Interrupt Handlers.
 *	[NR_DEFINT_INT - NR_DEFINT_EXEC - 1] => Deferred Execution Handlers.
 *
 */
typedef unsigned long defint_id_t;

/*
 * defint_exec:
 *
 * 	Routine called to execute all 
 * 	pending deferred work
 * 	on behalf of the Interrupt handlers
 * 	or other kernel code.
 */
extern void defint_exec( void );

/*
 * defint_exec_selected:
 * 
 * 	Routine called to execute
 * 	all pending deferred work based
 * 	on a subtype.  This allows
 * 	certain routines to be called
 * 	at specific intervals.
 */
extern void
defint_exec_selected ( defint_subtype_t type );

/*
 * defint_install_handler:
 *
 * 	Routine to install a handler.
 *
 * 	id	=> If this is DEFINT_IDALLOC, the system allocates
 * 	           an id for the handler and the
 * 	           parameter returns the id that should be used.
 * 	           The allocation is based on the priority:
 *
 *                 0 => [0  -  7] range
 *                 1 => [8  - 15] range
 *                 2 => [16 - 23] range
 *                 3 => [24 - 31] range
 *
 * 	           If the slots are full for a priority,
 *                 then we lower the priority.
 *                 Thus, the priority is only
 *                 a hint, not mandatory.
 *
 * 		   If this is NOT DEFINT_IDALLOC, then the system
 * 		   will attempt to use the specified id.
 * 		   The id number is important because
 * 		   it allows the user to control when
 * 		   the handler is to be run.
 * 		   No attempt to allocate an id will
 * 		   be done if the system is specified
 * 		   an id.
 *
 * 		   If there is a problem with id allocation,
 * 		   DEFINT_IDINVAL is returned.
 * 		   
 *		   For DEFINT_EXEC type, id's range from [ NR_DEFINT_INT to NR_DEFINT    -1 ].
 *		   For DEFINT_INT  type, id's range from [ 0             to NR_DEFINT_INT-1 ].
 *
 * 	priority=> DEFINT_PRI[00-03] from extermely urgent to lesser
 * 		   urgent.  Defferred Execution Handlers are
 * 		   always executed after Defferred Interupt Handlers.
 *
 * 	type	=> DEFINT_EXEC: These handlers have parameters
 * 		   that may be passed to them, and are run
 * 		   sequentially.
 *
 * 		   DEFINT_INT:	These handlers have no parameers
 * 		   and are run atomically.
 *
 * 		   if(type==DEFINT_INT)  => one parameter must be provided:
 *
 * 			[1] void (*defint_t)(void )
 *
 *		   if(type==DEFINT_EXEC) => three parameters must be provided:
 *
 *			[1] void (*defint_exec_t)(void *) entry point,
 *			[2] void * arguments
 *			[3] defint_subtype_t
 *			
 *		   The subtype allows the caller to control when
 *		   the Deferred Execution handler is to be run.
 *		   Up to 65535 different types maybe installed.
 *		   These are only run when the defint_exec_selected 
 *		   routine is called.
 *
 *	Example:
 *
 * 	...
 *
 * 	defint_id_t   id = 0;
 * 	defint_pri_t pri = DEFINT_PRI_00;
 *
 *	defint_install_handler( DEFINT_EXEC, &id, pri, e_entry, args, DEFINT_TIMER );
 *
 * 	id = DEFINT_IDALLOC;
 * 	defint_pri_t pri = DEFINT_PRI_01;
 *
 * 	defint_install_handler( DEFINT_INT, &id, pri, i_entry );
 * 	...
 *
 */
extern defint_status_t defint_install_handler( defint_type_t type,
					       defint_id_t   *id,
					       defint_pri_t  priority,
					       ... );

/*
 * defint_uninstall_handler:
 *	
 *	Routine to remove a handler. 
 */
extern defint_status_t defint_uninstall_handler( defint_id_t id );

/*
 * defint_enable:
 *
 * 	Routine called to allow the execution of a handler.
 */
extern defint_status_t defint_schedule ( defint_id_t id );

/*
 * defint_disable:
 *
 * 	Routine called to prevent the execution of a handler.
 */
extern defint_status_t defint_unschedule ( defint_id_t id );

/*
 * defint_inuse:
 *
 * 	Routine called to indicate the handler
 * 	is in use.  This prevents others from calling
 * 	the handler.
 */
extern defint_status_t defint_inuse ( defint_id_t id );

#endif
