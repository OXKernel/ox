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
 * Copyright Roger George Doss. All Rights Reserved.
 **************************************************************
 *
 *		<ox/irq_dispatch_tab.h>
 *
 *		Declares the interrupt handler table.
 *
 *************************************************************/
#ifndef _OX_IRQ_DISPATCH_TAB_H
#define _OX_IRQ_DISPATCH_TAB_H  1
#ifdef __cplusplus
 extern "C" {
#endif

/*
 * NOTE:
 *   The order in which these routines are
 *   placed in the call table **MUST**
 *   equal the order in which they are numbered.
 */
extern interrupt_handler_t
irq_dispatch_tab[Nr_IRQ];

#ifdef __cplusplus
 }
#endif
#endif /* _OX_IRQ_DISPATCH_TAB_H */
