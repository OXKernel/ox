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
 * @module  tss.h
 *
 *          contains the definition of the TSS
 *          (TASK STATE STRUCTURE) for Intel 32bit x86, the tss
 *          is the process's context. This module is part
 *          of the platform specific code.
 */
#ifndef _PLATFORM_I386_TSS_H
#define _PLATFORM_I386_TSS_H 1
#ifdef __cplusplus
 extern  "C" {
#endif

/* Intel floating point microprocessor
 * context, Intel programmer's manual, figure 7-13
 */
struct i387 {
    long cwd;		/* control word    */
    long swd;		/* status word     */
    long twd;		/* tag word        */
    long fip;		/* instr ptr       */
    long fcs;		/* code selector   */
    long foo;		/* op ptr offset   */
    long fos;		/* op ptr selector */
    long st_space[20];  /* stack space     */
			/* 80 bytes for FP-reg */
};

/* Intel task state structure, as defined
 * in the developer's manual, figure 6-2
 */
struct tss {
     long previous_task_link;       /* bits 31-16 are zero */
     long esp0;
     long  ss0;                     /* bits 31-16 are zero */
     long esp1;
     long  ss1;                     /* bits 31-16 are zero */
     long esp2;
     long  ss2;                     /* bits 31-16 are zero */
     long cr3;
     long eip;
     long eflags;
     long eax;
     long ecx;
     long edx;
     long ebx;
     long esp;
     long ebp;
     long esi;
     long edi;
     long  es;                      /* bits 31-16 are zero */
     long  cs;                      /* bits 31-16 are zero */
     long  ss;                      /* bits 31-16 are zero */
     long  ds;                      /* bits 31-16 are zero */
     long  fs;                      /* bits 31-16 are zero */
     long  gs;                      /* bits 31-16 are zero */
     long ldt;
     long io_map_base_address;      /* 0: trace bit, bitmap: 16-31 */
     struct i387 i387;
};

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_TSS_H */
