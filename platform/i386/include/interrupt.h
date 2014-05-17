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
 *  <platform/i386/interrupt.h>
 *
 *	C routines for initializing interrupts, using
 *	i8259 controllers
 *
 *******************************************************/
#ifndef _PLATFORM_I386_INTERRUPT_H
#define _PLATFORM_I386_INTERRUPT_H 1

#define Nr_IRQ		16	/* Number of irq vectors allowable. 
				 * See ./platform/asm_core/interrupt.h and 
				 * ./platform/protected_mode_defs.h.
				 */

/* Versioning information for each IRQ. */
typedef struct {
	unsigned short major, minor;
}irq_version_t;

/* This structure is used by the IRQ mechanism to 
 * obtain data from the respective drivers/handlers
 * regarding the drivers/ISRs.
 */
typedef struct {
	char 	*name, 
		*descr;
	irq_version_t version;
}irq_info_t;

/* Pointer to interrupt_handler. */
typedef int (*interrupt_handler_t) (int );

/* Pointer to interrupt info. */
typedef irq_info_t (*interrupt_info_t) ( void );

/* Used by ISR to indicate whether or not
 * a given IRQ should be enabled.
 */
typedef enum { IRQ_DISABLE, IRQ_ENABLE } irq_stat_t;


/* Used by driver to indicate whether or not
 * an IRQ will be shared.
 */
typedef enum { IRQ_EXCL, IRQ_SHARED } irq_type_t;

/* IRQ usage.
 */
__clinkage__
void interrupt_init( void );

__clinkage__
irq_stat_t interrupt_default_irq( int irq );

__clinkage__
void interrupt_install_handler ( int irq,
				                 irq_type_t type,
                                 interrupt_handler_t handler,
				                 interrupt_info_t    info );

__clinkage__
void interrupt_uninstall_handler ( int irq,
	       			   interrupt_handler_t handler );

/* IRQ probing. */
typedef void (*irq_probe_primer_t)( void );

__clinkage__
void irq_probe ( irq_probe_primer_t primer );

__clinkage__
bool irq_probe_done( unsigned *irq );

__clinkage__
bool irq_probe_inuse( int irq, irq_type_t *type );

#endif /* PLATFORM_I386_INTERRUPT_H */
