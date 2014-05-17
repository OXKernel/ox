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
/*
 * @file:
 *      pit.c
 *
 * @description:
 *      Device driver for Programmable Interrupt Timer (PIT).
 *
 * @author:
 *      Dr. Roger G. Doss, PhD
 *
 *      Some codes based on GazOS Operating System
 *      Copyright (C) 1999  Gareth Owen <gaz@athene.co.uk>
 *
 *      And http://www.osdever.net/bkerndev/Docs/pit.htm
 *
 */
#include <ox/defs.h>
#include <ox/types.h>
#include <ox/linkage.h>
#include <ox/error_rpt.h>
#include <ox/bool_t.h>

#include <platform/interrupt.h>
#include <platform/interrupt_admin.h>
#include <platform/protected_mode_defs.h>
#include <platform/asm_core/interrupt.h>
#include <platform/asm_core/util.h>
// #include <platform/drivers/include/pit.h> // TODO - Add this file here.
#include <drivers/chara/pit.h>

#include <asm_core/io.h>
#include <ox/types.h>

static int pit_mode = 0;
static int tick     = 0;
static interrupt_handler_t pit_handlers_tab[2] = {0};

void init_pit(float hz, unsigned char channel)
{
    unsigned int temp=0;

    temp = 1193180/hz;

    io_outb(TMR_CTRL, (channel*0x40) + TMR_BOTH + TMR_MD3);
    io_outb((0x40+channel), (unsigned char) temp);
    io_outb((0x40+channel), (unsigned char) (temp>>8));
}

unsigned int pit_getchannel(unsigned char channel)
{
    unsigned int x=0;

    io_outb(TMR_CTRL, (channel*0x40) + TMR_LATCH);
    x = io_inb(0x40+channel);
    x += (io_inb(0x40+channel) << 8);
    return x;
}

static
irq_stat_t pit_handler(int irq)
{
    interrupt_handler_t handler = NULL;
    ++tick;
    // Launch once a second...
    if(tick % 18 == 0) {
        if(pit_mode == PIT_SCHEDULER) {
            // Handler for os scheduler.
            handler = pit_handlers_tab[0];
            if(handler) {
#ifdef _PIT_DEBUG
		        printk("line %d file %s pit_mode == PIT_SCHEDULER\n",
				    __LINE__, __FILE__);
#endif
                return (*handler)(irq);
            } else {
                panic("pit_handler invalid pit_mode [0] is null");
            }
        } else if (pit_mode == PIT_DELAYCALIB) {
            // Handler for delay calibration.
            handler = pit_handlers_tab[1];
            if(handler) {
#ifdef _PIT_DEBUG
		        printk("line %d file %s pit_mode == PIT_DELAYCALIB\n",
				    __LINE__, __FILE__);
#endif
                return (*handler)(irq);
            } else {
                panic("pit_handler invalid pit_mode [1] is null");
            }
        } else {
            /* Default to debug mode. */
            printk("PIT Second has elapsed\n");
        }
    }
    return IRQ_ENABLE;
}

void pit_install_handler(int _pit_mode, interrupt_handler_t handler)
{
    asm_disable_interrupt();
    if(_pit_mode == PIT_SCHEDULER) {
        pit_handlers_tab[0] = handler;
        pit_mode = _pit_mode;
        printk("pit_install_handler installed PIT_SCHEDULER\n");
    } else if(_pit_mode == PIT_DELAYCALIB) {
        pit_handlers_tab[1] = handler;
        pit_mode = _pit_mode;
        printk("pit_install_handler installed PIT_DELAYCALIB\n");
    } else if(_pit_mode == PIT_DEBUG) {
        pit_mode = _pit_mode;
    } else {
        panic("pit_install_handler invalid _pit_mode");
    }
    asm_enable_interrupt();
}

/*
 * pit_enable:
 *
 * Call: 
 *  [1] pit_install_handler
 *  [2] pit_enable
 *
 * By default should default to debug (print a message every 1 second).
 * If we want 100hz, then ticks should be tick % 100, and 
 * init_pit should be called with 100.0, 0.
 *
 * This is from http://www.osdever.net/bkerndev/Docs/pit.htm.
 *
 */
irq_info_t pit_info()
{
    irq_info_t info;
    info.name  = "pit";
    info.descr = "OX Kernel PIT Driver";
    info.version.major = 1;
    info.version.minor = 0;
    return info;
}

void pit_enable()
{
    printk("initializing PIT driver version 1.0\n");
    // Launch 18 times per second...
    // so ticks++; ticks % 18 == 1 second.
    init_pit(18.2, 0);
    interrupt_install_handler(0,
                              IRQ_EXCL,
                              pit_handler,
                              pit_info);
    printk("done initializing PIT driver version 1.0\n");
    /* irq_enable(0); */
    enable_irq(0);
}

void pit_disable()
{
    printk("disabling PIT driver version 1.0\n");
    /* irq_disable(0); */
    disable_irq(0);
    printk("done disabling PIT driver version 1.0\n");
}
