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
 * <sys/sigcontext.h>
 * The sigcontext structure is used by sigreturn()
 * which is not  called by the user,
 * but is used internally by the signal catching mechanism.
 * it is defined in:-
 *   <platform/machine/sigcontext.h>
 *  
 ********************************************************/
#ifndef  _SYS_SIGCONTEXT_H
#define  _SYS_SIGCONTEXT_H  1
#ifdef __cplusplus
 extern "C" {
#endif
#include <platform/sigcontext.h>

#define SC_SIGCONTEXT  2 /* non-zero == include signal context */
#define SC_NOREGLOCALS 4 /* non-zero == registers not saved/stored */

extern int sigreturn (unsigned long __unused);
#ifdef __cplusplus
 }
#endif
#endif /* _SYS_SIGCONTEXT_H */ 
