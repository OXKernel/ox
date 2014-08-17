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
 *  <gdt_init.h>
 *	    
 *       C routies and macros to initialize the GDT and IDT
 *       on intel x86
 */
#ifndef __GDT_IDT_INIT_H
#define __GDT_IDT_INIT_H

void initilize_idt(void);
void init_vector(void *intr_handler, unsigned char intr_num, unsigned short attrs, unsigned int selector);

/*
 *  descriptor type
 */

#define D_LDT   0x200   /* LDT segment        */
#define D_TASK  0x500   /* Task gate          */
#define D_TSS   0x900   /* TSS                */
#define D_CALL  0x0C00  /* 386 call gate      */
#define D_INT   0x0E00  /* 386 interrupt gate */
#define D_TRAP  0x0F00  /* 386 trap gate      */
#define D_DATA  0x1000  /* Data segment       */
#define D_CODE  0x1800  /* Code segment       */


/*
 *  Descriptors may include the following:
 */

#define D_DPL3         0x6000   /* DPL3 or mask for DPL                    */
#define D_DPL2         0x4000   /* DPL2 or mask for DPL                    */
#define D_DPL1         0x2000   /* DPL1 or mask for DPL                    */
#define D_PRESENT      0x8000   /* Present                                 */
#define D_NOT_PRESENT  0x8000   /* Not Present                             */
                                /* Note, the PRESENT bit is set by default */
                                /* Include NOT_PRESENT to turn it off      */
                                /* Do not specify D_PRESENT                */


/*
 *  Segment descriptors (not gates) may include:
 */

#define D_ACC      0x100  /* Accessed (Data or Code)          */

#define D_WRITE    0x200  /* Writable (Data segments only)    */
#define D_READ     0x200  /* Readable (Code segments only)    */
#define D_BUSY     0x200  /* Busy (TSS only)                  */

#define D_EXDOWN   0x400  /* Expand down (Data segments only) */
#define D_CONFORM  0x400  /* Conforming (Code segments only)  */

#define D_BIG      0x40	  /* Default to 32 bit mode           */
#define D_BIG_LIM  0x80	  /* Limit is in 4K units             */


/*
 * GDT entry for x86
 * reference: http://wiki.osdev.org/Global_Descriptor_Table
 */
union desc_entry {
	struct {
		unsigned long long dummy;
	};
	struct {
		unsigned short limit_low;     /* limit 0..15    */
		unsigned short base_low;      /* base  0..15    */
		unsigned char base_mid;       /* base  16..23   */
		unsigned char access;         /* access byte    */
		unsigned int limit_high:4;    /* limit 16..19   */
		unsigned int flags:4;         /* flags Gr, Sz   */
		unsigned char base_high;      /* base 24..31    */
	};
} __attribute__ ((packed));

/*
 * IDT entry for x86
 * reference: http://wiki.osdev.org/Interrupt_Descriptor_Table
 */
struct gate_entry {
   unsigned short offset_low;   /* offset 0..15    */
   unsigned short selector;     /* selector        */
   unsigned char zero;          /* unused          */
   unsigned char type_attr;     /* fields GateType, S, DPL, P */
   unsigned short offset_high;  /* offset 16..31   */
} __attribute__ ((packed));


/*
 * Macros to initialize individual descriptor entry and gate entry.
 */
#define INIT_DESC(flags_in, base, limit) \
	{.limit_low = (limit & 0xffff)}, \
	{.base_low = (base & 0xffff)}, \
	{.base_mid = ((base >> 16) & 0xff)}, \
	{.access = ((flags_in + D_PRESENT) >> 8)}, \
	{.limit_high = (limit >> 16)}, \
	{.flags = ((flags_in & 0xff) >> 4)}, \
	{.base_high = (base >> 24)}
#endif
