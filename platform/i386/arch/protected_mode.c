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
/************************************************************ 
 * Copyright(C) Roger George Doss. All Rights Reserved.
 ************************************************************
 * @module  protected_mode.c
 *	    
 *          contains code for protected mode operation under
 *          intel 386
 */
#include <ox/linkage.h>
#include <ox/types.h>

#include <platform/segment.h>
#include <platform/segment_defs.h>
#include <platform/segment_selectors.h>

#include <platform/protected_mode.h>
#include <platform/protected_mode_defs.h>

#include <platform/asm_core/exception.h>
#include <platform/asm_core/interrupt.h>
#include <platform/asm_core/syscall.h>

/* declare descriptor tables 
 * NOTE: Initializing these tables to zero
 * 	 will result in a large executable
 * 	 because the data will be set to zero.
 */
struct segment_descriptor GDT[ Nr_GDT ]={0};
struct load_descriptor __GDT__ = {(Nr_GDT * sizeof (struct segment_descriptor) - 1),GDT};

struct segment_descriptor IDT[ Nr_IDT ]={0};
struct load_descriptor __IDT__ = {(Nr_IDT * sizeof (struct segment_descriptor) - 1),IDT};

// RGDDEBUG was 32
#ifdef _USE_NEW_OFFSET
#   define IRQ_VECTOR(x) \
		(x) + 0x68

#   define IRQ_VECTOR_S(x) \
		(x) + 0x70
#else
#   define IRQ_VECTOR(x) \
		(x) + 32

#   define IRQ_VECTOR_S(x) \
		(x) + 32
#endif

/* internal call table containing addresses
 * of pure assembly procedures for handling
 * exceptions and interrupts
 */
static struct excpt_int_tab {

	void (*address)(void);		/* address of routine in the kernel */
	unsigned char vector_number;	/* vector number for IDT	    */
	unsigned char privilege;	/* 0,3, Kernel or User		    */

}EXCPT_INT_TAB[] = {
	/* exceptions are loaded first
	 * followed by interrupts, see 5-20
	 * of Intel developer's manual
	 */
	{excpt_divide_error		  ,0,	 0},
	{excpt_debug	  		  ,1,	 0},	/* single-step exception */
	{excpt_nmi	 		  ,2,	 0},
	{excpt_int3	  		  ,3,	 3},	/* break point exception */
	{excpt_overflow			  ,4,	 0},
	{excpt_bounds			  ,5,	 0},
	{excpt_invalid_operation 	  ,6,	 0},
	{excpt_device_not_available	  ,7,	 0},	/* math co-processor N/A */
	{excpt_double_fault		  ,8,	 0},
	{excpt_coprocessor_segment_overrun ,9,	 0},
	{excpt_invalid_TSS		  ,10,	 0},
	{excpt_segment_not_present	  ,11,	 0},
	{excpt_stack_segment		  ,12,	 0},
	{excpt_general_protection	  ,13,	 0},	/* lucky 13		*/
	{excpt_page_fault		  ,14,	 0},
    /* 15 is reserved, see http://en.wikipedia.org/wiki/Interrupt_descriptor_table */
	{excpt_reserved			  ,15,	 0},	/* machine-check 	*/
	{excpt_coprocessor_error	  ,16,	 0},	/* floating point	*/
	{excpt_alignment_check		  ,17,	 0},
	{excpt_coprocessor_error,18,	 0},	/* floating point error	*/

	{irq_00		     ,IRQ_VECTOR(0),	 0},
	{irq_01		     ,IRQ_VECTOR(1),	 0},
	{irq_02		     ,IRQ_VECTOR(2),	 0},
	{irq_03		     ,IRQ_VECTOR(3),	 0},
	{irq_04		     ,IRQ_VECTOR(4),	 0},
	{irq_05		     ,IRQ_VECTOR(5),	 0},	
	{irq_06		     ,IRQ_VECTOR(6),	 0},
	{irq_07		     ,IRQ_VECTOR(7),	 0},
	{irq_08		     ,IRQ_VECTOR_S(8),	 0},
	{irq_09		     ,IRQ_VECTOR_S(9),	 0},
	{irq_10		     ,IRQ_VECTOR_S(10),	 0},
	{irq_11		     ,IRQ_VECTOR_S(11),	 0},
	{irq_12		     ,IRQ_VECTOR_S(12),	 0},
	{irq_13		     ,IRQ_VECTOR_S(13),	 0},
	{irq_14		     ,IRQ_VECTOR_S(14),  0},
	{irq_15		     ,IRQ_VECTOR_S(15),	 0},
};

__clinkage__
struct segment_descriptor *get_segment_descriptor(unsigned i)
{
    return &GDT[i];
}// get_segment_descriptor

/* protect_init:
 * initializes system for protected mode operation
 */
__clinkage__
void protect_init( void )
{
	int i;
	/* set-up the system for protected mode operation
	 * a. first entries in GDT are initialized, the rest are set to zero
	 * b. init process would be the only task initialized at entry 8,9
	 *    this will be done later ( ie, when we have an init task )
	 * c. IDT is initialized with kernel interrupt/exception handlers
	 * d. IDT entry 80 has system call handler installed
	 */
	/* GDT[0] == NULL, GDT[1] == not used
	 */
#ifdef _VM_ENABLED

	/* GDT[2] == kernel code segment
	 * base at high virtual memory at 0xC0000000, 1Gig limit
	 */ 
	protect_init_codesegment(&GDT[2],0xC0000000,0x3FFFF,0x0);
	/* GDT[3] == kernel data-stack segment
	 * base at high virtual memory at 0xC0000000, 1Gig limit
	 */
	protect_init_datasegment(&GDT[3],0xC0000000,0x3FFFF,0x0);
	/* GDT[4] == user code segment
	 * base at low virtual memory at 0x0, 3Gig limit
	 */
	protect_init_codesegment(&GDT[4],0x0,0xBFFFF,0x3);
	/* GDT[5] == user data-stack segment
	 * base at low virtual memory at 0x0, 3Gig limit
	 */
	protect_init_datasegment(&GDT[5],0x0,0xBFFFF,0x3);
#elif _VM_DISABLED

	/* this is to test if what we have works...
	 * both kernel and user apps have same address space hear :-(
     * address space starts at 0x0 and ends at 0xFFFFF which is 
     * 1 meg * 4096 pages or 4 GIG address space.
	 */
	protect_init_codesegment(&GDT[1],0x0,0xFFFFF,0x0);
	protect_init_codesegment(&GDT[2],0x0,0xFFFFF,0x0);
	/* GDT[3] == kernel data-stack segment
	 */
	protect_init_datasegment(&GDT[3],0x0,0xFFFFF,0x0);
	/* GDT[4] == user code segment
	 */
	protect_init_codesegment(&GDT[4],0x0,0xFFFFF,0x3);
	/* GDT[5] == user data-stack segment
	 */
	protect_init_datasegment(&GDT[5],0x0,0xFFFFF,0x3);
#else
#	error "protected_mode.c: GDT not initialized"
#endif

	/* GDT[6] == not used, GDT[7] == not used
	 */
	/* set-up init task
	 */
	/* set-up IDT
	 * traps/exceptions followed by irq's
	 */
#ifndef _USE_ALTERNATIVE_IDT
	printk("protect_init:: initializing IDT\n");
	initialize_idt();
#else
	printk("protect_init:: initializing alternative IDT\n");
	for (i = 0; i < Nr_EXCPT_HNDLR; i++) {
		protect_init_trapdesc(EXCPT_INT_TAB[i].vector_number,
				      (unsigned int)EXCPT_INT_TAB[i].address,
				      EXCPT_INT_TAB[i].privilege);
	}

	int count_HNDLR = Nr_INT_HNDLR + Nr_EXCPT_HNDLR;
	for (i = Nr_EXCPT_HNDLR; i < count_HNDLR; i++) {
		protect_init_intdesc(EXCPT_INT_TAB[i].vector_number,
			      	     (unsigned int)EXCPT_INT_TAB[i].address,
				     EXCPT_INT_TAB[i].privilege);
	}
	/* set-up system call handler at int 80 */
	protect_init_intdesc(SYSCALL_INT, (unsigned int)&syscall_handler, 0x3);
#endif

}/* protect_init */

/* protect_init_codesegment:
 * initializes code segment
 */
__clinkage__
inline
void protect_init_codesegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,
                               unsigned char privilege )
{
	/* initialize as a system descriptor */
	protect_init_systemdesc (psd,base,limit);
	/* set as system descriptor */
	_SET_DESC_SYSTEM(psd,_DESC_CSSEG);
	/* mark as present */
	_SET_DESC_PRESENT(psd,_DESC_PRESENTF);
	/* set privilege, either kernel or user mode */
	_SET_DESC_DPL(psd,privilege);
	/* set default operation as 32bit segments */
	_SET_DESC_DB(psd,_DESC_DEFAULT_OP);
	/* set type as executable, read-only */
	_SET_DESC_TYPE(psd,_DESC_CODESEG | _DESC_EXECREADONLY);

}/* protect_init_codesegment */

/* protect_init_datasegment:
 * initializes datasegment
 */
__clinkage__
inline
void protect_init_datasegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,   
                               unsigned char privilege )
{
	/* initialize as a system descriptor */
	protect_init_systemdesc(psd,base,limit);
	_SET_DESC_SYSTEM (psd,_DESC_CSSEG);
	/* mark as present */
	_SET_DESC_PRESENT(psd,_DESC_PRESENTF);
	/* set privilege, either kernel or user mode */
	_SET_DESC_DPL(psd,privilege);
	/* set default operation as 32bit segments */
	_SET_DESC_DB(psd,_DESC_DEFAULT_OP);
	/* set type as read-write */
	_SET_DESC_TYPE(psd,_DESC_DATASEG | _DESC_READWRITE);

}/* protect_init_datasegment */

/* protect_init_systemdesc:
 * initializes a system descriptor
 */
__clinkage__
inline
void protect_init_systemdesc( register
                              struct segment_descriptor *psd,
                              unsigned int base,
                              unsigned int limit )
{
	/* set descriptor base,limit,granularity */
	_SET_DESC_BASE(psd,base);
	_SET_DESC_LIMIT(psd,limit);
	_SET_DESC_GRAN(psd,_DESC_GRANULARITY);

}/* protect_init_systemdesc */

/* protect_init_stacksegment:
 * initializes a stack segment
 */
__clinkage__
inline
void protect_init_stacksegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,   
                               unsigned char privilege )
{
	/* initialize as a system descriptor */
	protect_init_systemdesc(psd,base,limit);
	_SET_DESC_SYSTEM(psd,_DESC_CSSEG);
	/* mark as present */
	_SET_DESC_PRESENT(psd,_DESC_PRESENTF);
	/* set privilege, either kernel or user mode */
	_SET_DESC_DPL(psd,privilege);
	/* set default operation as 32bit segments */
	_SET_DESC_DB(psd,_DESC_DEFAULT_OP);
	/* set type as read-write */
	_SET_DESC_TYPE(psd,_DESC_DATASEG | _DESC_READWRITE | _DESC_EXPANDDOWN);

}/* protect_init_stacksegment */

/* protect_init_tsssegment:
 * initializes a TSS segment
 */
__clinkage__
inline
void protect_init_tsssegment(  register
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,	/* must be >= 0x67, sizeof struct TSS in struct process */
			       unsigned char privilege )
{
	/* see figure 6-3 of Intel developer's manual */
	protect_init_systemdesc(psd,base,limit);
	/* system descriptor */
	_SET_DESC_SYSTEM(psd,_DESC_SYSSEG);
	/* mark as present */
	_SET_DESC_PRESENT(psd,_DESC_PRESENTF);
	/* privilege level */
	_SET_DESC_DPL(psd,privilege);
	/* TSS descriptor */
	_SET_DESC_TYPE(psd,_DESC_TSS);

}/* protect_init_tsssegment */

/* protect_init_ldtsegment:
 * initialize an LDT segment
 */
__clinkage__
inline
void protect_init_ldtsegment( register
			      struct segment_descriptor *psd,
			      unsigned int base,
			      unsigned int limit,
			      unsigned char privilege )
{
	protect_init_systemdesc(psd,base,limit);
	/* system descriptor */
	_SET_DESC_SYSTEM(psd,_DESC_SYSSEG);
	/* mark as present */
	_SET_DESC_PRESENT(psd,_DESC_PRESENTF);
	/* privilege level */
	_SET_DESC_DPL(psd,privilege);
	/* TSS descriptor */
	_SET_DESC_TYPE(psd,_DESC_LDT);

}/* protect_init_ldtsegment */


/* protect_init_intdesc:
 * initializes interrupt descriptor
 */
__clinkage__
inline
void protect_init_intdesc  ( unsigned vector_number,
                             unsigned int base,
                             unsigned char privilege )
{

	/* see figure 5-2 of Intel developer's manual */
	_SET_DESC_SYSTEM(&IDT[vector_number],_DESC_SYSSEG);
	_SET_DESC_PRESENT(&IDT[vector_number],_DESC_PRESENTF);
	_SET_DESC_DPL(&IDT[vector_number],privilege);
	_SET_DESC_TYPE(&IDT[vector_number],_DESC_INTERRUPT);
	_SET_DESC_OFFSET(&IDT[vector_number],base);
	_SET_DESC_SELECTOR(&IDT[vector_number],KERNEL_CS);

}/* protect_init_intdesc */


/* protect_init_trapdesc:
 * initializes a trap descriptor, exception
 * handler
 */
 __clinkage__
inline
void protect_init_trapdesc  ( unsigned vector_number,
			      unsigned int base,
			      unsigned char privilege )
{
	/* see figure 5-2 of Intel developer's manual */
        _SET_DESC_SYSTEM(&IDT[vector_number],_DESC_SYSSEG);
        _SET_DESC_PRESENT(&IDT[vector_number],_DESC_PRESENTF);
        _SET_DESC_DPL(&IDT[vector_number],privilege);
        _SET_DESC_TYPE(&IDT[vector_number],_DESC_TRAP);
        _SET_DESC_OFFSET(&IDT[vector_number],base);
        _SET_DESC_SELECTOR(&IDT[vector_number],KERNEL_CS);	
}/* protect_init_trapdesc */

/*
 * EOF
 */
