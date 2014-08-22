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
 * Author: Arun Prakash Jana <engineerarun at gmail dot com>
 ************************************************************
 *  <gdt_init.c>
 *	    
 *       C routies and macros to initialize the GDT and IDT
 *       on intel x86
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

#include <platform/8259.h>
#include <platform/gdt_init.h>

void init_null(void);
asm (
	".globl init_null        \n"
	"init_null:              \n"
	"iret                    \n" 
);

void load_idt(struct gate_entry *IDT)
{
	asm volatile ("lidt %0"::"m" (*IDT));
}

union desc_entry __GDT[100] =
{
	{.dummy = 0x0},
	INIT_DESC((D_CODE + D_READ + D_BIG + D_BIG_LIM), 0, 0xFFFFF),
	INIT_DESC((D_DATA + D_WRITE + D_BIG + D_BIG_LIM), 0, 0xFFFFF),
	INIT_DESC((D_TSS + D_BIG), 8192, 103),
	INIT_DESC((D_TSS + D_BIG), 12288, 103),
	INIT_DESC((D_LDT + D_BIG), 0, (2 * sizeof(union desc_entry) - 1)),
};

struct
{
	unsigned short limit __attribute__ ((packed));
	union desc_entry *idt __attribute__ ((packed));
} loadgdt = {(100 * sizeof(union desc_entry) - 1), __GDT};

struct gate_entry __IDT[256];

struct
{ 
  unsigned short limit __attribute__ ((packed));
  struct gate_entry *idt  __attribute__ ((packed)); 
} loadidt = {(256 * sizeof(struct gate_entry) - 1), __IDT};

void initialize_idt(void)
{
	int count = 0;
	unsigned short common_attrs = D_PRESENT + D_INT + D_DPL3;

	for (count = 19; count < 256; count++)
		init_vector(init_null, count, common_attrs, KERNEL_CS);

	common_attrs = D_PRESENT + D_TRAP + D_DPL3;
	init_vector(excpt_divide_error, 0, common_attrs, KERNEL_CS);
	init_vector(excpt_debug, 1, common_attrs, KERNEL_CS);
	init_vector(excpt_nmi, 2, common_attrs, KERNEL_CS);
	init_vector(excpt_int3, 3, common_attrs, KERNEL_CS);
	init_vector(excpt_overflow, 4, common_attrs, KERNEL_CS);
	init_vector(excpt_bounds, 5, common_attrs, KERNEL_CS);
	init_vector(excpt_invalid_operation, 6, common_attrs, KERNEL_CS);
	init_vector(excpt_device_not_available, 7, common_attrs, KERNEL_CS);
	init_vector(excpt_double_fault, 8, common_attrs, KERNEL_CS);
	init_vector(excpt_coprocessor_segment_overrun, 9, common_attrs, KERNEL_CS);
	init_vector(excpt_invalid_TSS, 10, common_attrs, KERNEL_CS);
	init_vector(excpt_segment_not_present,11, common_attrs, KERNEL_CS);
	init_vector(excpt_stack_segment, 12, common_attrs, KERNEL_CS);
	init_vector(excpt_general_protection, 13, common_attrs, KERNEL_CS);
	init_vector(excpt_page_fault, 14, common_attrs, KERNEL_CS);
	/* interrupt 15 is reserved, http://en.wikipedia.org/wiki/Interrupt_descriptor_table */
	init_vector(excpt_reserved, 15, common_attrs, KERNEL_CS);
	init_vector(excpt_coprocessor_error, 16, common_attrs, KERNEL_CS);
	init_vector(excpt_alignment_check, 17, common_attrs, KERNEL_CS);
	init_vector(excpt_coprocessor_error, 18, common_attrs, KERNEL_CS);

	// Setup IRQ handling.
	common_attrs = D_PRESENT + D_INT + D_DPL3;
	init_vector(irq_00, M_VEC+0, common_attrs, KERNEL_CS);
	init_vector(irq_01, M_VEC+1, common_attrs, KERNEL_CS);
	init_vector(irq_02, M_VEC+2, common_attrs, KERNEL_CS);
	init_vector(irq_03, M_VEC+3, common_attrs, KERNEL_CS);
	init_vector(irq_04, M_VEC+4, common_attrs, KERNEL_CS);
	init_vector(irq_05, M_VEC+5, common_attrs, KERNEL_CS);
	init_vector(irq_06, M_VEC+6, common_attrs, KERNEL_CS);
	init_vector(irq_07, M_VEC+7, common_attrs, KERNEL_CS);

	init_vector(irq_08, S_VEC+0, common_attrs, KERNEL_CS);
	init_vector(irq_09, S_VEC+1, common_attrs, KERNEL_CS);
	init_vector(irq_10, S_VEC+2, common_attrs, KERNEL_CS);
	init_vector(irq_11, S_VEC+3, common_attrs, KERNEL_CS);
	init_vector(irq_12, S_VEC+4, common_attrs, KERNEL_CS);
	init_vector(irq_13, S_VEC+5, common_attrs, KERNEL_CS);
	init_vector(irq_14, S_VEC+6, common_attrs, KERNEL_CS);
	init_vector(irq_15, S_VEC+7, common_attrs, KERNEL_CS);

	init_vector(syscall_handler, SYSCALL_INT, common_attrs, KERNEL_CS);

	load_idt(&(__IDT[0]));
}

void init_vector(void *intr_handler, unsigned char intr_num, unsigned short attrs, unsigned short selector)
{

	__IDT[intr_num].offset_low    = (unsigned short) (((unsigned long)intr_handler) & 0xffff);
	__IDT[intr_num].selector      = selector;
	__IDT[intr_num].zero          = 0x0;
	__IDT[intr_num].type_attr     = (attrs >> 8);
	__IDT[intr_num].offset_high   = (unsigned short) (((unsigned long)intr_handler) >> 16);
}


#if 0
void dumpregs()
{
  TSS *badtask;
  if(MULTASK){
    badtask = (GDT[task_queue[curtask].tss/8].desc.base_high << 24) | (GDT[task_queue[curtask].tss/8].desc.base_med << 16) | GDT[task_queue[curtask].tss/8].desc.base_low;
    kprint("########## DUMP ##########\n");
    kprint("CS=0x%H\tDS=0x%H\tES=0x%H\tFS=0x%H\tGS=0x%H\t\n",badtask->cs,badtask->ds,badtask->es,badtask->fs,badtask->gs);
    kprint("EIP=0x%H\tESI=0x%H\tEDI=0x%H\n",badtask->eip,badtask->eip,badtask->esi,badtask->edi);
    kprint("SS=0x%H\tEBP=0x%H\tESP=0x%H\n",badtask->ss,badtask->ebp,badtask->esp);
    kprint("EAX=0x%H\tEBX=0x%H\tECX=0x%H\tEDX=0x%H\n",badtask->eax,badtask->ebx,badtask->ecx,badtask->edx);
    kprint("EFLAGS=0x%H\tPDBR=0x%H\n",badtask->eflags,badtask->cr3);
    kprint("##########################\n");
  }
}

void redefinevectors()
{
  asm("cli");
  create_task(_int0,GATE,0);
  create_task(_int1,GATE,1);
  create_task(_int2,GATE,2);
  create_task(_int3,GATE,3);
  create_task(_int4,GATE,4);
  create_task(_int5,GATE,5);
  create_task(_int6,GATE,6);
  create_task(_int7,GATE,7);
  create_task(_int8,GATE,8);
  create_task(_int9,GATE,9);
  create_task(_int10,GATE,10);
  create_task(_int11,GATE,11);
  create_task(_int12,GATE,12);
  create_task(_int13,GATE,13);
  create_task(_int14,GATE,14);
  create_task(_int15,GATE,15);
  create_task(_int16,GATE,16);
    asm("sti");
}

void _int0()
{
  kprint("\n\nint0: Divide Error\n");
  dumpregs();	
	while(1);
}
void _int1()
{
  kprint("\n\nint1: Debug exception\n");
  dumpregs();	
	while(1);
}
void _int2()
{
  kprint("\n\nint2: unknown error\n");
  dumpregs();
	while(1);
}
void _int3()
{
  kprint("\n\nint3: Breakpoint\n");
  dumpregs();
	while(1);
}
void _int4()
{
  kprint("\n\nint4: Overflow\n");
  dumpregs();
	while(1);
}
void _int5()
{
  kprint("\n\nint 5: Bounds check\n");
  dumpregs();
	while(1);
}
void _int6()
{
  kprint("\n\nint6: Invalid opcode\n");
  dumpregs();
	while(1);
}
void _int7()
{
  kprint("\n\nint7: Coprocessor not available\n");
  dumpregs();
	while(1);
}
void _int8()
{
  kprint("\n\nint8: Double fault\n");
  dumpregs();
  while(1);
}
void _int9()
{
  kprint("\n\nint9: Coprocessor Segment overrun\n");
  dumpregs();
	while(1);
}
void _int10()
{
  kprint("\n\nint10: Invalid TSS\n");
  dumpregs();
  while(1);
}
void _int11()
{
  kprint("\n\nint11: Segment not present\n");
  dumpregs();
	while(1);
}
void _int12()
{
  kprint("\n\nint12: Stack exception\n");
  dumpregs();
	while(1);
}
void _int13()
{
  kprint("\n\nint13: General Protection Exception \n");
  dumpregs();	
  while(1);
}
void _int14()
{
  kprint("\n\nint14: Page fault\n");
  dumpregs();
	while(1);
}
void _int15()
{
  kprint("\n\nint15: Unknown error\n");
  dumpregs();
	while(1);
}
void _int16()
{
  kprint("\n\nint16: Coprocessor error\n");
  dumpregs();
	while(1);
}
#endif
