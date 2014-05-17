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
 * Copyright (C) Roger George Doss. All Rights Reserved.
 *************************************************************
 *
 * @module 
 *	   interrupt.c
 *
 * @description
 *         Contains code for initializing Intel interrupt
 *         controller, currently only i8259 is supported,
 *         no Advanced Interrupt Controller Support (APIC).
 */
#define _POSIX_SOURCE 1
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>

#include <ox/linkage.h>
#include <ox/error_rpt.h>
#include <ox/bool_t.h>

#include <platform/interrupt.h>
#include <platform/interrupt_admin.h>
#include <platform/protected_mode_defs.h>
#include <platform/asm_core/interrupt.h>
#include <platform/asm_core/util.h>

interrupt_handler_t 	irq_dispatch_tab [Nr_IRQ];
// Table of function pointers.
static interrupt_info_t irq_info_tab [Nr_IRQ];

#define Nr_IRQ_SLOT	2
#define Nr_IRQ_ELEM	(Nr_IRQ * Nr_IRQ_SLOT)
#define Nr_ADMIN_ELEM ((Nr_IRQ + Nr_IRQ_ELEM) -1)	/* Total number of elements possible. */
#define IRQ_INVALID	-1

typedef struct {
	int irq;				/* IRQ associated with handler. */
	interrupt_handler_t handler;		/* Shared handler address. */
	interrupt_info_t    info;		/* Information about the handler. */
}shared_handler_t;

static unsigned char IRQ_TYPE_MASK[Nr_IRQ];	/* IRQ_EXCL == 0, so all are init to this. */
static shared_handler_t IRQ_SHARED_TAB[Nr_IRQ_ELEM];
static int nr_shared;

static
irq_stat_t shared_handler( int irq );

static int nr_admin;
const irq_admin_t *irq_get_data( void )
{
	register int i,j,k;
	static irq_admin_t admin[Nr_ADMIN_ELEM];

	/* Organize the data in a readable manner. */
	for(i=0; i < Nr_IRQ; i++) {
		/* Exclusive IRQs first. */
		if(IRQ_TYPE_MASK[i]==IRQ_EXCL) {
			admin[i].address = (unsigned long)irq_dispatch_tab[i];
			admin[i].type    = IRQ_EXCL;
			admin[i].irq     = i;
			admin[i].info 	 = (*irq_info_tab[i])();
		}
	}
	/* i is now a place holder in the admin table. */
	++i;
	for(j=0; j < Nr_IRQ; j++) {
		if(IRQ_TYPE_MASK[i]==IRQ_SHARED) {
			for(k=0; k < Nr_IRQ_ELEM; k++) {
				if(IRQ_SHARED_TAB[k].irq == j) {
					admin[i].address = (unsigned long)&IRQ_SHARED_TAB[k].handler;
					admin[i].type	 = IRQ_SHARED;
					admin[i].irq	 = i;
					admin[i].info	 = (*IRQ_SHARED_TAB[k].info)();
					++i;
				}
			}
		}
	}

	/* Record how many IRQ handlers there are. */
	nr_admin = i;
	return (admin);

}/* irq_get_data */

void irq_free_data( const irq_admin_t *admin )
{
	/* If the admin table ends up being dynamic,
	 * this is a place holder to removing it.
	 */
}/* irq_free_data */

void irq_print_data( const irq_admin_t *admin )
{
	register int i;
	printk("--OX-- EXCL IRQ HANDLERS --:\n");
	for(i=0; i < nr_admin; i++,admin++) {
		if(admin->type==IRQ_SHARED) {
			break;
		}
		printk( "irq : (%u)\t"
			"addr: (%x)\n"
		        "name: (%s)\t"
			"desc: (%s)\t"
			"vers: (%u.%u)\t\n",
				admin->irq,
				admin->address,
				admin->info.name,
				admin->info.descr,
				admin->info.version.major,
				admin->info.version.minor);
	}
	printk("--OX-- SHARED IRQ HANDLERS --:\n");
	for(; i < nr_admin; i++, admin++) {
		printk( "irq : (%u)\t"
			"addr: (%x)\n"
		        "name: (%s)\t"
			"desc: (%s)\t"
			"vers: (%u.%u)\t\n",
				admin->irq,
				admin->address,
				admin->info.name,
				admin->info.descr,
				admin->info.version.major,
				admin->info.version.minor);
	}

}/* irq_print_data */

static
void install_shared( int irq, interrupt_handler_t handler, interrupt_info_t info )
{
	/* Use a simplified hash algorithm with circular queue:
	 * install at offsets, and scan, in a circular
	 * manner for collisions.
	 */
    register int pass = 0, installed = 0;
	register shared_handler_t *ptr = &IRQ_SHARED_TAB[irq * Nr_IRQ_SLOT];
	register shared_handler_t *end = &IRQ_SHARED_TAB[Nr_IRQ_ELEM-1];
	register shared_handler_t *beg = &IRQ_SHARED_TAB[0];

	disable_irq(irq);
	if(IRQ_TYPE_MASK[irq]==IRQ_EXCL && 
	    irq_dispatch_tab[irq] == interrupt_default_irq) {
		/* This is the initial state of the IRQ, allow
		 * it to be set to SHARED.
		 */ 
		IRQ_TYPE_MASK[irq] = IRQ_SHARED;
	}
	if(IRQ_TYPE_MASK[irq] != IRQ_SHARED) {
		panic("install_shared: can not convert IRQ_EXCL to IRQ_SHARED\n");
	}
	if(nr_shared == (Nr_IRQ_ELEM -1 )) {
		panic("install_shared: IRQ_SHARED_TAB full.\n");
	}

	while(true) {
		if(ptr->irq == IRQ_INVALID) {
			/* found a slot */
			ptr->irq = irq;
			ptr->handler=handler;
			ptr->info=info;
			nr_shared++;
            installed = 1;
			break;
		}
		ptr++;
		if(ptr == end) {
			/* maintain circular list */
			ptr=beg;
            pass++;
		}
        if(pass == 2) {
            break;
        }
	}

    if(!installed) {
        panic("install_shared: failed to find slot\n");
    }

	/* install our shared handler */
	irq_dispatch_tab[irq] = shared_handler;
	irq_info_tab[irq] = NULL;

}/* install_shared */

static
void uninstall_shared( int irq, interrupt_handler_t handler )
{
	register shared_handler_t *ptr = &IRQ_SHARED_TAB[irq * Nr_IRQ_SLOT];
	register shared_handler_t *end = &IRQ_SHARED_TAB[Nr_IRQ_ELEM-1];
	register shared_handler_t *beg = &IRQ_SHARED_TAB[0];
	register shared_handler_t *str = ptr;

	disable_irq(irq);
	if(!nr_shared) {
		/* none installed */
		return;
	}
	do {
		if(ptr->irq == irq && !handler) {
			/* delete all handlers for this irq */
			ptr->irq        = IRQ_INVALID;
			ptr->handler    = interrupt_default_irq;
			ptr->info	= NULL;
			nr_shared--;
		}
		else if(ptr->irq == irq && ptr->handler == handler) {
			/* delete handler */
			ptr->irq 	= IRQ_INVALID;
			ptr->handler	= interrupt_default_irq;
			ptr->info	= NULL;
			nr_shared--;
			break;
		}
		ptr++;
		if(ptr == end) {
			/* maintain circular list */
			ptr=beg;
		}
	}while( ptr != str );

	if(!handler) {
		IRQ_TYPE_MASK[irq] 	= IRQ_EXCL;
		irq_dispatch_tab[irq] 	= interrupt_default_irq;
		irq_info_tab[irq]	= NULL;
	}

}/* uninstall_shared */

static
irq_stat_t shared_handler( int irq )
{
	register shared_handler_t *ptr = &IRQ_SHARED_TAB[irq * Nr_IRQ_SLOT];
	register shared_handler_t *end = &IRQ_SHARED_TAB[Nr_IRQ_ELEM-1];
	register shared_handler_t *beg = &IRQ_SHARED_TAB[0];
	register shared_handler_t *str = ptr;
	irq_stat_t ret = IRQ_DISABLE;

	if(!nr_shared) {
		panic("shared_handler: IRQ not shared\n");
	}

	do {
		if(ptr->irq == irq) {
			if((*ptr->handler)(irq)== IRQ_ENABLE) {
				/* if at least one shared handler wants
				 * the IRQ enabled, we must enable it
				 */
				ret = IRQ_ENABLE;
			}
		}
		ptr++;
		if(ptr == end) {
			/* maintain circular list */
			ptr=beg;
		}
	}while( ptr != str );
	return (ret);

}/* shared_handler */

static bool PROBING;				/* Protect against pilot error. */
static unsigned irq_probe_mask, in_use_mask;
static unsigned FLAGS;
static interrupt_handler_t     irq_tmp [Nr_IRQ];

static
irq_stat_t probe_handler( int irq )
{
	/* Called after an interrupt has occured,
	 * mark the approberiate bit in the mask.
	 */
      	irq_probe_mask |= (1 << irq);
	/* The return does not really matter,
	 * as we will restore the system state prior
	 * to probing anyhow.
	 */
	return ( IRQ_DISABLE );

}/* probe_handler */

__clinkage__
void irq_probe ( irq_probe_primer_t primer )
{
	int i;

	/* Just in case the caller did not
	 * call irq_probe_done().
	 */
	if(PROBING) {
		panic("irq_probe: inconsistent system state\n");
	}

	/* Save the computer state. */
	FLAGS=asm_get_eflags();

	/* Stop interrupts. */
	asm_disable_interrupt();

	/* Save which IRQs are enabled/disabled. */
	irq_save_i8259_state();
	/* Turn all IRQs on. */
	irq_i8259_enable_all();

	for(i=0; i < Nr_IRQ; i++) {
		/* Save IRQ dispatch table. */
		irq_tmp[i] = irq_dispatch_tab[i];
		/* Build in_use_mask. */
		/* NOTE: If the IRQ is shared, we allow it to be
		 *	 reported.
		 */
		if(IRQ_TYPE_MASK[i]==IRQ_EXCL && 
		   irq_dispatch_tab[i]!=interrupt_default_irq) {
			in_use_mask |= (1 << i);
		}
		/* Install probe_handler. */
		irq_dispatch_tab[i] = probe_handler;
	}

	/* Let the caller perform any operations or 
	 * I/O prior to enabling interrupts.
	 */
	if(primer) {
		(*primer)();
	}

	/* Let the interrupt occur. */
	asm_enable_interrupt();

}/* irq_probe */

__clinkage__
bool irq_probe_done( unsigned *irq )
{
	int i,_irq = IRQ_INVALID;
	bool ret   = false;

	if(!irq) {
		return (false);
	}

	/* Stop interrupts. */
	asm_disable_interrupt();

	/* Mask out IRQs that we are already
	 * using.  These are IRQs that have
	 * a handler installed and are marked IRQ_EXCL.
	 * If the shared handler was installed prior
	 * to the probe, we are permitted to consider
	 * this as a possible IRQ provided that no
	 * other IRQ was triggerred in the process.
	 */
	irq_probe_mask = (irq_probe_mask & (~in_use_mask));

	/* Scan the probe mask. */
	for(i=0; i < Nr_IRQ; i++) {
		if(irq_probe_mask & (1 << i)) {
			if(_irq != IRQ_INVALID) {
				ret = false;
			}
			else {
				_irq = i;
				ret  = true;
			}
		}
	}

	/* Restore dispatch table. */
	for(i=0; i < Nr_IRQ; i++) {
		irq_dispatch_tab[i] = irq_tmp[i];
	}

	/* Restore i8259 controller state. */
	irq_restore_i8259_state();

	/* Restore system state prior to probing. */
	/* NOTE: We do not enable interrupts, the caller should. */
	asm_set_eflags(FLAGS);

	/* We have the IRQ, now restore our state. */
	if(ret==true) {
		*irq = _irq;
	}
	else {
		*irq = IRQ_INVALID;
	}

	/* Reset the probe mask. */
	irq_probe_mask = 0x0;
	in_use_mask= 0x0;
	PROBING = false;
	return (ret);

}/* irq_probe_done */

/*
 * irq_probe_inuse:
 *  	If inuse, returns true and 
 *  	reports the type of the IRQ handler installed.
 *	Otherwise, returns false.
 */
__clinkage__
bool irq_probe_inuse( int irq, irq_type_t *type )
{
	if(PROBING) {
		panic("irq_probe_inuse: Inconsistent state\n");
	}
	if(irq < 0 || irq >= Nr_IRQ) {
		panic("irq_probe_inuse: Invalid IRQ\n");
	}
	if(irq_dispatch_tab[irq] == interrupt_default_irq) {
		/* Not in use. */
		return (false);
	}
	if(irq_dispatch_tab[irq] == shared_handler) {
		*type = IRQ_SHARED;
		return (true);
	}

       *type = IRQ_EXCL;
	return (true);

}/* irq_probe_inuse */

void init_8259();
/* interrupt_init:
 * initializes i8259 controller
 */
__clinkage__
void interrupt_init( void )
{
	int i;
	/* initialize programmable interrupt controller
	 * using assembler code
	 */ 
	// irq_init_ctl(); RGDDEBUG
	/* assign default irq handler on initialization
	 */
	for(i=0; i < Nr_IRQ; i++) {
		irq_dispatch_tab[i] 	= interrupt_default_irq;
	}
	for(i=0; i < Nr_IRQ_ELEM; i++) {
		IRQ_SHARED_TAB[i].irq 	= IRQ_INVALID;
		IRQ_SHARED_TAB[i].handler=interrupt_default_irq;
	}
    printk("interrupt_init:: initializing\n"); // RGDDEBUG
    init_8259();

}/* interrupt_init */

/* interrupt_default_irq:
 * 
 * Handles an interrupt sent during
 * system initialization;
 * this is technically provided to handle a bug
 * found under some older hardware.
 * The kernel panics if the irq number is invalid
 * otherwise, a message is printed.
 *
 */
__clinkage__
irq_stat_t interrupt_default_irq( int irq )
{
	/* stop kernel operation if irq value
	 * is not in the allowed range
	 */
	if ((irq < 0) || (irq >= Nr_IRQ)) {
		panic("interrupt_default_irq: invalid IRQ (%d)\n",irq);
	}

	/* otherwise, print error and continue
	 */ 
	printk("interrupt_default_irq: IRQ (%d) \n",irq);
	return (IRQ_ENABLE);

}/* interrupt_default_irq */

/* interrupt_install_handler: 
 * installs an interrupt handler 
 * in the vector slot associated with it
 */
__clinkage__
void interrupt_install_handler( int irq,
				irq_type_t type,
				interrupt_handler_t handler,
				interrupt_info_t    info )
{
	if(PROBING) {
		panic("interrupt_install_handler: Inconsistent state\n");
	}
	if( (irq < 0) || (irq >= Nr_IRQ) ) {
	 	panic("interrupt_install_handler: invalid IRQ (%d)\n",irq);
	}
	if(type == IRQ_SHARED) {
		install_shared(irq,handler,info);
	}
	else if (type == IRQ_EXCL) {
		if(IRQ_TYPE_MASK[irq] == IRQ_SHARED) {
			panic("interrupt_install_handler: can not convert IRQ_SHARED to IRQ_EXCL\n");
		}
		if(irq_dispatch_tab[irq] == handler) {
			/* already installed */
			return;
		}
		if( irq_dispatch_tab[irq] != interrupt_default_irq ){
			panic("interrupt_install_handler: invalid handler installed\n");
		}
		/* disable irq prior to installing
	 	 * a handler
	 	 */
		disable_irq(irq);
		irq_dispatch_tab[irq] = handler;
		irq_info_tab[irq] = info;
        printk("irq_dispatch_tab [%d]\n",irq_dispatch_tab);
        printk("interrupt_install_handler:: installed [%d] at irq [%d]\n",irq_dispatch_tab[irq],irq);
		/* enabling the irq is left up to the driver/isr
	 	 * as it may want to do other setups 
	 	 */
	}
	else {
		panic("interrupt_install_handler: invalid IRQ type (%d)\n",type);
	}

}/* interrupt_install_handler */

__clinkage__
void interrupt_uninstall_handler ( int irq, 
			 	   interrupt_handler_t handler )
{
	if(PROBING) {
		panic("interrupt_uninstall_handler: Inconsistent state\n");
	}
	if( (irq < 0) || (irq >= Nr_IRQ) ) {
	 	panic("interrupt_uinstall_handler: invalid IRQ (%d)\n",irq);
	}
	if( irq_dispatch_tab[irq] == interrupt_default_irq ) {
		/* already disabled */
	}
	if(IRQ_TYPE_MASK[irq] == IRQ_SHARED) {
		uninstall_shared(irq, handler);
	}
	else if (IRQ_TYPE_MASK[irq] == IRQ_EXCL) {
		/* disable irq prior to uninstalling
 	 	 * a handler
 	 	 */
		disable_irq(irq);
		irq_dispatch_tab[irq] 	= interrupt_default_irq;
		irq_info_tab[irq] 	= NULL;
	}

}/* interrupt_uninstall_handler */

static unsigned int irq_mask = 0xFFFF;
#define M_PIC  0x20     /* I/O for master PIC              */
#define M_IMR  0x21     /* I/O for master IMR              */
#define S_PIC  0xA0     /* I/O for slave PIC               */
#define S_IMR  0xA1     /* I/O for slace IMR               */

#define EOI    0x20     /* EOI command                     */

#define ICW1   0x11     /* Cascade, Edge triggered         */
                        /* ICW2 is vector                  */
                        /* ICW3 is slave bitmap or number  */
#define ICW4   0x01     /* 8088 mode                       */

#define M_VEC  0x68     /* Vector for master               */
#define S_VEC  0x70     /* Vector for slave                */

#define OCW3_IRR  0x0A  /* Read IRR                        */
#define OCW3_ISR  0x0B  /* Read ISR                        */

void init_8259(void)
{
   io_outb(M_PIC, ICW1);       /* Start 8259 initialization    */
   io_outb(S_PIC, ICW1);

   io_outb(M_PIC+1, M_VEC);    /* Base interrupt vector        */
   io_outb(S_PIC+1, S_VEC);

   io_outb(M_PIC+1, 1<<2);     /* Bitmask for cascade on IRQ 2 */
   io_outb(S_PIC+1, 2);        /* Cascade on IRQ 2             */

   io_outb(M_PIC+1, ICW4);     /* Finish 8259 initialization   */
   io_outb(S_PIC+1, ICW4);

   io_outb(M_IMR, 0xff);       /* Mask all interrupts          */
   io_outb(S_IMR, 0xff);
}

// RGDDEBUG
void enable_irq(unsigned short irq_no)
{
    irq_mask &= ~(1 << irq_no);
    if(irq_no >= 8)
        irq_mask &= ~(1 << 2);

    //printk(" file %s line %d mask %d\n",__FILE__,__LINE__, irq_mask & 0xFF);
    io_outb(M_PIC+1, irq_mask & 0xFF);
    if(irq_no >= 8) // RGDDEBUG
        io_outb(S_PIC+1, (irq_mask >> 8) & 0xFF);
}

/* disables irq irq_no */
void disable_irq(unsigned short irq_no)
{
    irq_mask |= (1 << irq_no);
    if((irq_mask & 0xFF00)==0xFF00)
        irq_mask |= (1 << 2);

    io_outb(M_PIC+1, irq_mask & 0xFF);
    if(irq_no >= 8) // RGDDEBUG
        io_outb(S_PIC+1, (irq_mask >> 8) & 0xFF);
}

#if 0
/* This is for debugging, its from Linux. */
static unsigned char cache_21 = 0xFF;
static unsigned char cache_A1 = 0xFF;

void enable_irq(unsigned short irq_nr)
{
    unsigned long flags;
    unsigned char mask;
    mask = ~(1 << (irq_nr & 7));
    printk(" file %s line %d\n",__FILE__,__LINE__);
    //flags = asm_get_eflags();
    printk(" file %s line %d\n",__FILE__,__LINE__);
    if(irq_nr < 8) {
        printk(" file %s line %d\n",__FILE__,__LINE__);
        //asm_disable_interrupt();
        cache_21 &= mask;
        printk(" file %s line %d mask %d\n",__FILE__,__LINE__,cache_21);
        io_outb(M_PIC+1, cache_21);
        //asm_set_eflags(flags);
        printk(" file %s line %d\n",__FILE__,__LINE__);
        return;
    }
    printk(" file %s line %d\n",__FILE__,__LINE__);
    //asm_disable_interrupt();
    cache_A1 &= mask;
    io_outb(S_PIC+1,cache_A1);
    //asm_set_eflags(flags);
}
#endif

/*
 * EOF
 */
