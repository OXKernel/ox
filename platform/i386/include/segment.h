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
 * Copyright (C) Roger George Doss. All Rights Reserved.
 **************************************************************
 * @module  segment.h
 *
 *          contains various data structures
 *          needed for segment descriptors, including macros
 *          for manipulating bits in the GDT,LDT and IDT tables.
 *          This module is part of the platform specific code.
 *          All bit masks are specified in hex, all bit
 *          positions, are specified in decimal.
 */
#ifndef _PLATFORM_I386_SEGMENT_H
#define _PLATFORM_I386_SEGMENT_H 1
#include <ox/defs.h>
#ifdef __cplusplus 
 extern "C" {
#endif

/* The following data structure is
 * the Intel x86 protected
 * mode, hardware specific,
 * segment descriptor and it
 * must be 64bits long, its format
 * is defined by Intel in figure 3-8 of the
 * developer's manual as follows:-
 *
 * longword 1 := 31:24   23 22 21 20  19:16  15 14:13 12 11:8 7:0
 *               Base    G  D/B 0 AVL limit   P  DPL   S TYPE Base
 *               (32:24)              (19:16)                 (23:16)
 *
 * longword 0 := 31:16  15:0
 *               Base   limit
 *               (15:0) (15:0)
 *
 */
struct segment_descriptor {
    unsigned long lw0,lw1;
};

/*
 * Describes system descriptor tables to
 * the CPU
 */
struct load_descriptor {
	unsigned short limit      	__attribute__ ((packed));
	struct segment_descriptor *gdt  __attribute__ ((packed));
};

/* The following data structure is the
 * GDT (GLOBAL DESCRIPTOR TABLE), it
 * contains all segments in the system
 * and is initialized and loaded on startup.
 */
extern
struct segment_descriptor GDT[];

/* The following data structure is the 
 * IDT (INTERRUPT DESCRIPTOR TABLE), it
 * contains the segments for all interrupt
 * handling, it is initialized and loaded 
 * on startup.
 */
#ifndef _USE_GAZ_IDT
extern
struct segment_descriptor IDT[];
#endif


/* The following macros are designed to
 * simplify the reading of special bits in
 * a segment_descriptor.
 */

/* _DESC_AVL:
 * Available bits
 */
inline
unsigned int _DESC_AVL(struct segment_descriptor *desc);

/* _DESC_BASE:
 * Segment base Address
 */
inline
unsigned int _DESC_BASE(struct segment_descriptor *desc);

/* _DESC_DB:
 * Default operation, always 1 for 32bit
 */
inline
unsigned int _DESC_DB(struct segment_descriptor *desc);

/* _DESC_DPL:
 * Descriptor Privaledge Level
 */
inline
unsigned int _DESC_DPL(struct segment_descriptor *desc);

/* _DESC_GRAN:
 * Descriptor Granularity, 1 for
 * 4KB granualrity
 */
inline
unsigned int _DESC_GRAN(struct segment_descriptor *desc);

/* _DESC_LIMIT:
 * Descriptor Limit, indicates the size
 * of a segment descriptor
 */
inline
unsigned int _DESC_LIMIT(struct segment_descriptor *desc);

/* _DESC_PRESENT:
 * Indicates wheather a descriptor is present
 * in memory when its value is 1
 */
inline
unsigned int _DESC_PRESENT(struct segment_descriptor *desc);

/* _DESC_SYSTEM:
 * Indicates wheather a descriptor is a system
 * descriptor if its value is 0, 1 indicates
 * user code/data. If it is a system descriptor,
 * it can be of various types: ldt,task,interrupt,
 * call-gate,trap-gate
 */
inline
unsigned int _DESC_SYSTEM(struct segment_descriptor *desc);

/* _DESC_TYPE:
 * Indicates the various types that a descriptor
 * may be, this differentiates the various types
 * of user and system descriptors, but does not
 * distinguish if the segment is of system or
 * user type
 */
inline
unsigned int _DESC_TYPE(struct segment_descriptor *desc);

inline
unsigned int _DESC_OFFSET(struct segment_descriptor *desc);

inline
unsigned int _DESC_SELECTOR(struct segment_descriptor *desc);
	
/* The following macros are designed to simplify
 * writing of special bits within a segment_descriptor
 */
inline
void _SET_DESC_AVL(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_BASE(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_DB(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_DPL(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_GRAN(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_LIMIT(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_PRESENT(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_SYSTEM(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_TYPE(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_OFFSET(struct segment_descriptor *desc,unsigned int data);

inline
void _SET_DESC_SELECTOR(struct segment_descriptor *desc,unsigned int data);

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_SEGMENT_H */
