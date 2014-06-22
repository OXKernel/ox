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
 * 	@module 
 *		ox_main.c
 *
 *	@description
 *		The OX kernel is loaded by the boot program,
 *	which in turn calls _start, the assembly language
 *	entry point of the OS.  _start calls main(),
 *	the machine-dependent entry point,
 *	which in turn calls ox_main() the kernel-dependent
 *	entry point.
 *		Returning non-zero indicates error, which
 *	causes us to panic.
 */
#include <ox/error_rpt.h>
#include <ox/linkage.h>
#include <ox/fs.h>
#include <ox/fs/fs_syscalls.h>
#include <ox/fs/compat.h>
#include <ox/fs/init.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <ox/types.h>
#include <ox/defs.h>
#include <platform/protected_mode_defs.h>
#include <platform/segment.h>
#include <platform/tss.h>
#include <platform/asm_core/util.h>

#include <platform/interrupt.h>
#include <drivers/chara/delay.h>
#include <drivers/chara/keyboard.h>
#include <drivers/chara/pit.h>
#include <ox/process.h>
#include <ox/scheduler.h>
#include <ox/mm/page.h>

extern int INTERRUPT_COUNTER;
extern void raise_software_interrupt();

static
irq_stat_t pit_test_handler(int irq)
{
    static int ticks = 0;
    ++ticks;
    if(ticks == 100) {
        printk("pit_test_handler called !\n");
        ticks = 0;
    }
    return IRQ_ENABLE;
}

__clinkage__
int ox_main (int argc, char **argv)
{
    unsigned long delay_count = 0, i = 0, disk_size = 0, start_sector = 0;
    char ch = 0, tmp = 0;
    char *addr = 209715200;
    int print = 0;
    // TODO - Do other system initializations here.
    //      - init of i8259 controller first.
    //        This happens in start.s, see 
    //        call interrupt_init
    //        which sets up the handlers to point
    //        to the default IRQ handler.
    //
    //        **Research how the i8259 is initialized,
    //          look at the assembler code in the asm_core**
    //
    //        **Do keyboard_enable first** and call keyboard_getch()
    //        to see if we can echo back what we get to the
    //        terminal.
    //
#ifdef _TEST_KEYBOARD
    keyboard_enable();
    printk(" file %s line %d\n",__FILE__,__LINE__);
    //asm_disable_interrupt();
    asm_enable_interrupt();
    //raise_software_interrupt();
    printk("interrupt_count [%d]\n",INTERRUPT_COUNTER);
#if 0
    while((ch=keyboard_getch()) != '1') {
        print = 1;
        if(ch == 0)
            print = 0;
        if(ch >= '0' && ch <= '9')
            tmp = keyboard_getch();
        if(tmp >= '0' && tmp <= '9')
            ch = tmp;
        if(print)
            printk("recieved [%c]\n", ch);
        print = 1;
    }
#endif
    while((ch=keyboard_getch()) != '1') {
        printk("recieved [%c]\n", ch);
    }
#endif

#ifdef _TEST_HIGH_MEM_WRITE
   *addr = '1'; 
   printk("addr [%c]\n",*addr);
#endif

//#ifdef _TEST_MEM_INIT
    printk("ox_main:: start initializing memory\n");
    mem_init();
    printk("ox_main:: done  initializing memory\n");
//#endif

//#ifdef _TEST_PAGE_ALLOC
   *addr = '1'; 
   printk("addr [%c]\n",*addr);
   // Test page allocation and setting of read-only pages.
   // Try setting read-only on kernel page.
   addr = page_alloc(1);
   printk(" ADDR = [%d]\n",addr);
   // mem_set_read_only(addr,1);
   printk("Should crash here\n");
   *addr = '2';
   printk("Didn't crash\n");
   printk("addr [%c]\n",*addr);
   *addr = 0;
   printk("Didn't crash\n");

   page_free(addr,1);
   addr = page_alloc(4);
   printk("addr := %d\n",addr);
   if(addr == 0) {
       printk(" file %s line %d\n",__FILE__,__LINE__);
       for(;;) /* halt */;
   }
   for(i = 0; i < (4 * 4096); ++i) {
        addr[i] = (i % 128);
        //if(addr[i] < 0) addr[i] = -addr[i];
   }
   ch = 0;
   for(i = 0; i < (4 * 4096); ++i) {
        if(addr[i] != (i % 128)) {
            printk("i [%d] addr [%d] i mod 128 [%d]\n",i,addr[i],i%128);
            ch = 1;
            break;
        }
   }
   if(ch == 1) {
        printk("memory failed!\n");
   } else {
        printk("memory allocated successfully!\n");
   }
   page_free(addr,4);

   addr = page_alloc(8);
   printk("addr := %d\n",addr);
   for(i = 0; i < (4 * 4096); ++i) {
        addr[i] = (i % 128);
        //if(addr[i] < 0) addr[i] = -addr[i];
   }
   ch = 0;
   for(i = 0; i < (4 * 4096); ++i) {
        if(addr[i] != (i % 128)) {
            printk("i [%d] addr [%d] i mod 128 [%d]\n",i,addr[i],i%128);
            ch = 1;
            break;
        }
   }
   if(ch == 1) {
        printk("memory failed!\n");
   } else {
        printk("memory allocated successfully!\n");
   }
   printk(" file %s line %d\n",__FILE__,__LINE__);
   page_free(addr,8);
   printk(" file %s line %d\n",__FILE__,__LINE__);
   //
   // now test kernel memory allocation
   //
   addr = kpage_alloc(4);
   printk("addr := %d\n",addr);
   for(i = 0; i < (4 * 4096); ++i) {
        addr[i] = (i % 128);
        //if(addr[i] < 0) addr[i] = -addr[i];
   }
   printk(" file %s line %d\n",__FILE__,__LINE__);
   ch = 0;
   for(i = 0; i < (4 * 4096); ++i) {
        if(addr[i] != (i % 128)) {
            printk("i [%d] addr [%d] i mod 128 [%d]\n",i,addr[i],i%128);
            ch = 1;
            break;
        }
   }
   if(ch == 1) {
        printk("memory failed!\n");
   } else {
        printk("memory allocated successfully!\n");
   }
   printk(" file %s line %d\n",__FILE__,__LINE__);
   kpage_free(addr,4);

   printk(" file %s line %d\n",__FILE__,__LINE__);
   addr = kpage_alloc(8);
   printk("addr := %d\n",addr);
   for(i = 0; i < (4 * 4096); ++i) {
        addr[i] = (i % 128);
        //if(addr[i] < 0) addr[i] = -addr[i];
   }
   printk(" file %s line %d\n",__FILE__,__LINE__);
   ch = 0;
   for(i = 0; i < (4 * 4096); ++i) {
        if(addr[i] != (i % 128)) {
            printk("i [%d] addr [%d] i mod 128 [%d]\n",i,addr[i],i%128);
            ch = 1;
            break;
        }
   }
   if(ch == 1) {
        printk("memory failed!\n");
   } else {
        printk("memory allocated successfully!\n");
   }
   printk(" file %s line %d\n",__FILE__,__LINE__);
   kpage_free(addr,8);
   printk(" file %s line %d\n",__FILE__,__LINE__);

//#endif

#ifdef _TEST_MALLOC

    printk("testing malloc allocator\n");
    malloc_unit_test();
    printk("testing kmalloc allocator\n");
    kmalloc_unit_test();

#endif

#ifdef _TEST_DISK_SIZE
   printk("testing ata_disk_size\n");
   ide_enable();
   asm_enable_interrupt();
   disk_size = ata_disk_size(0,&start_sector);
   printk("disk_size [%d] start_sector [%d]\n", disk_size, start_sector);
   printk("done testing ata_disk_size\n");
   printk("initializing file system\n");
   //ata_test_rw(10);
   fs_reset();
   if(fs_init(4 * (1<<20)) == FS_INIT_FAIL) {
        panic("ox_main:: error initializing file system\n");
   }
   printk("done initializing file system\n");
#endif

#ifdef _TEST_PIT
    // Note this effectively threads the kernel and
    // will get called whenever the timer interrupt goes
    // off while the main thread will sit at a halted
    // state.
    pit_install_handler(PIT_DELAYCALIB, pit_test_handler);
    pit_enable();
    asm_enable_interrupt();
#endif

    // Compute delay calibration.
    // This will setup pit_mode to be PIT_DELAYCALIB,
    // after that, call pit_install_handler(PIT_SCHEDULER, schedule_handler).
    // To set to scheduling.
    // After this is done, should be able to call delay()
    // to delay CPU. Should be useful in device drivers.
#ifdef _TEST_DELAY_CALIBRATE
    // Calculate for 1ms.
    pit_enable();
    asm_enable_interrupt();
    delay_count = calibrateDelayLoop();
    printk("delay_count = %d\n", delay_count);
#endif

#ifdef _TEST_SCHEDULER
    // Setup scheduling so we can multi-task.
    // NOTE: At current, we create an init task and child init task,
    // and we try to switch between these and the scheduler interrupt.
    // This appears to work. Context switching was changed from
    // TSS (which didn't work) to software switch. The software
    // switch works by calling into the task's EIP (start routine)
    // on initialization, and then switching stacks subsequently.
    // See kernel/scheduler.c and asm_core/scheduler.s.
    // A lot of work was spent getting something that doesn't crash
    // and there may be a lot of work to go...
    scheduler_init();
    pit_enable();
    asm_enable_interrupt();
#endif

    // TODO - Perhaps implement a rudimentary shell here.
    printk("halt...");
	for ( ; ; )
		/* wait */;

}/* ox_main */

/*
 * EOF
 */
