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
/***************************************************************
 * Copyright (C)  2000, Roger George Doss. All Rights Reserved.
 ***************************************************************
 *
 * 	@module
 *		<ox/syscall_dispatch_tab.h>
 *
 *	@description
 *
 *  		syscall_dispatch_tab.h contains the system call table
 *  	which formulates the user entry points into the
 *	 system.
 *
 *  	NOTE: 
 *	  - These must be placed in the table
 *          according to their respective call numbers
 *          in include/platform/[machine]/call.h
 *        - syscall_pointer_t is a typedef
 *          and defined within include/ox/types.h
 *          where we keep available kernel typedef's
 *        - we do not include kernel
 *          headers within one another, instead,
 *          we include them from within the implementation
 *          file in correct order
 *          eg,
 *            #include <ox/types.h>
 *	      #include <ox/defs.h>
 *            #include <ox/syscall.h>       
 *            #include <ox/syscall_dispatch_tab.h>
 *
 **************************************************************/
#ifndef _OX_SYSCALL_TAB_H
#define _OX_SYSCALL_TAB_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/* number of system calls 
 * implemented, must equal 
 * those noted in include/platform/[machine]/call.h
 * NOTE:
 *   The order in which these routines are
 *   placed in the call table **MUST**
 *   equal the order in which they are numbered.
 */
extern syscall_pointer_t
syscall_dispatch_tab[Nr_SYS_CALL];

#ifdef __cplusplus
 }
#endif
#endif /* _OX_SYSCALL_TAB_H */
