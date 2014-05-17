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
 * @module  segment.c
 *
 *          C code for manipulating a segment descriptor
 *          for Intel 386.
 */
#include <platform/segment.h>
#include <ox/linkage.h>

/* _DESC_AVL:
 * Available bits
 */
__clinkage__
inline
unsigned int _DESC_AVL(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 20) & 0x1);
}

/* _DESC_BASE:
 * Segment base Address
 */
__clinkage__
inline
unsigned int _DESC_BASE(struct segment_descriptor *desc)
{
        return ((((*desc).lw0 >> 16) & 0xFFFF) |
               (( (*desc).lw1        &   0xFF) << 16) |
	       ((((*desc).lw1 >> 24) &   0xFF) << 24));
}

/* _DESC_DB:
 * Default operation, always 1 for 32bit
 */
__clinkage__
inline
unsigned int _DESC_DB(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 22) & 0x1);
}

/* _DESC_DPL:
 * Descriptor Privaledge Level
 */
__clinkage__
inline
unsigned int _DESC_DPL(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 13) & 0x3);
}

/* _DESC_GRAN:
 * Descriptor Granularity, 1 for
 * 4KB granualrity
 */
__clinkage__
inline
unsigned int _DESC_GRAN(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 23) & 0x1);
}

/* _DESC_LIMIT:
 * Descriptor Limit, indicates the size
 * of a segment descriptor
 */
__clinkage__
inline
unsigned int _DESC_LIMIT(struct segment_descriptor *desc)
{
	return ((((*desc).lw0 & 0xFFFF) |
	       ((((*desc).lw1 >> 16) & 0xF) << 16))
	       & 0x000FFFFF);
}

/* _DESC_PRESENT:
 * Indicates wheather a descriptor is present
 * in memory when its value is 1
 */
__clinkage__
inline
unsigned int _DESC_PRESENT(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 15) & 0x1);
}

/* _DESC_SYSTEM:
 * Indicates wheather a descriptor is a system
 * descriptor if its value is 0, 1 indicates
 * user code/data. If it is a system descriptor,
 * it can be of various types: ldt,task,interrupt,
 * call-gate,trap-gate
 */
__clinkage__
inline
unsigned int _DESC_SYSTEM(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 12) & 0x1);
}

/* _DESC_TYPE:
 * Indicates the various types that a descriptor
 * may be, this differentiates the various types
 * of user and system descriptors, but does not
 * distinguish if the segment is of system or
 * user type
 */
__clinkage__
inline
unsigned int _DESC_TYPE(struct segment_descriptor *desc)
{
	return (((*desc).lw1 >> 8) & 0xF);
}

__clinkage__
inline
unsigned int _DESC_OFFSET(struct segment_descriptor *desc)
{
	return (((*desc).lw0 & 0xFFFF) |
	        ((*desc).lw1 & 0xFFFF0000));
}

__clinkage__
inline
unsigned int _DESC_SELECTOR(struct segment_descriptor *desc)
{
	return (((*desc).lw0 >> 16) & 0xFFFF);
}
	
/* The following macros are designed to simplify
 * writing of special bits within a segment_descriptor
 */
__clinkage__
inline
void _SET_DESC_AVL(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0x1 & (data)) << 20;
}

__clinkage__
inline
void _SET_DESC_BASE(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw0 |= (0xFFFF & (data)) << 16;
	(*desc).lw1 |= ((0xFF0000 & (data)) >> 16);
	(*desc).lw1 |= (0xFF000000 & (data));
}

__clinkage__
inline
void _SET_DESC_DB(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0x1 & (data)) << 22;
}

__clinkage__
inline
void _SET_DESC_DPL(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0x3 & (data)) << 13;
}

__clinkage__
inline
void _SET_DESC_GRAN(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0x1 & (data)) << 23;
}

__clinkage__
inline
void _SET_DESC_LIMIT(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw0 |= (0xFFFF & (data));
	(*desc).lw1 |= (0xF0000 & (data));
}

__clinkage__
inline
void _SET_DESC_PRESENT(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0x1 & (data)) << 15;
}

__clinkage__
inline
void _SET_DESC_SYSTEM(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0x1 & (data)) << 12;
}

__clinkage__
inline
void _SET_DESC_TYPE(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw1 |= (0xF & (data)) << 8;
}

__clinkage__
inline
void _SET_DESC_OFFSET(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw0 |= (0xFFFF & (data));
	(*desc).lw1 |= (0xFFFF0000 & (data));
}

__clinkage__
inline
void _SET_DESC_SELECTOR(struct segment_descriptor *desc,unsigned int data)
{
	(*desc).lw0 |= (0xFFFF & (data)) << 16;
}

/*
 * EOF
 */
