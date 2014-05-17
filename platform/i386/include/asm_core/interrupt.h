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
 *  <platform/i386/asm_core/interrupt.h>
 *
 *	prototypes for interrupt handlers
 *
 *******************************************************/
#ifndef _PLATFORM_I386_ASM_CORE_INTERRUPT_H
#define _PLATFORM_I386_ASM_CORE_INTERRUPT_H  1
#ifdef __cplusplus
 extern "C" {
#endif

extern
void irq_init_ctl( void );

/* irq_enable and irq_disable are
 * assembler codes.
 * All irq_ functions are implemented
 * in interrupt.s.
 */
extern
void irq_enable  (int irq);

extern
int  irq_disable (int irq);

/*
 * enable_irq, disable_irq are
 * C codes in interrupt.c.
 */
extern
void enable_irq(unsigned short irq_no);

extern
void disable_irq(unsigned short irq_no);

extern
void irq_save_i8259_state(void );

extern
void irq_i8259_enable_all(void );

extern
void irq_restore_i8259_state(void );

/* Entry points for IRQ handling. */
extern
void irq_00( void );

extern
void irq_01( void );

extern
void irq_02( void );

extern
void irq_03( void );

extern
void irq_04( void );

extern
void irq_05( void );

extern
void irq_06( void );

extern
void irq_07( void );

extern
void irq_08( void );

extern
void irq_09( void );

extern
void irq_10( void );

extern
void irq_11( void );

extern
void irq_12( void );

extern
void irq_13( void );

extern
void irq_14( void );

extern
void irq_15( void );

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_ASM_CORE_INTERRUPT_H */
