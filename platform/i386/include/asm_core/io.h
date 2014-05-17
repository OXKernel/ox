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
 * 
 * @module  <platform/i386/asm_core/io.h
 *	    
 *	    contains io routines for supporting devices
 *	    these are defined in arch/i386/asm_core/io.s
 *	    and are platform specific
 *
 *	    _p	 indicates 1x pause
 *	    _p_	 indicates 2x pause
 *	    _p__ indicates 3x pause
 */
#ifndef _PLATFORM_I386_ASM_CORE_IO_H
#define _PLATFORM_I386_ASM_CORE_IO_H 1
#ifdef __cplusplus
extern "C" {
#endif


/* char */
extern
unsigned char io_inb      ( unsigned int port );

extern
void io_outb              ( unsigned int port, unsigned char value );

/* short */
extern
unsigned short io_inw     ( unsigned int port );

extern
void io_outw              ( unsigned int port, unsigned short value );

/* long */
extern
unsigned long io_inl      ( unsigned int port );

extern
void io_outl              ( unsigned int port, unsigned long value );

/* char */
extern
unsigned char io_inb_p    ( unsigned int port );

extern
void io_outb_p            ( unsigned int port, unsigned char value );

/* short */
extern
unsigned short io_inw_p   ( unsigned int port );

extern
void io_outw_p            ( unsigned int port, unsigned short value );

/* long */
extern
unsigned long io_inl_p    ( unsigned int port );

extern
void io_outl_p            ( unsigned int port, unsigned long value );

/* char */
extern
unsigned char io_inb_p_   ( unsigned int port );

extern
void io_outb_p_           ( unsigned int port, unsigned char value );

/* short */
extern 
unsigned short io_inw_p_  ( unsigned int port );

extern
void io_outw_p_           ( unsigned int port, unsigned short value );

/* long */
extern
unsigned long io_inl_p_   ( unsigned int port );

extern
void io_outl_p_           ( unsigned int port, unsigned long value );

/* char */
extern
unsigned char io_inb_p__  ( unsigned int port );

extern
void io_outb_p__          ( unsigned int port, unsigned char value );

/* short */
extern
unsigned short io_inw_p__ ( unsigned int port );

extern
void io_outw_p__          ( unsigned int port, unsigned short value );

/* long */
extern
unsigned long io_inl_p__  ( unsigned int port );

extern
void io_outl_p__          ( unsigned int port, unsigned long value );


extern
void io_insb   ( unsigned int port, void *address, unsigned long count );

extern
void io_outsb  ( unsigned int port, void *address, unsigned long count );

extern
void io_insw   ( unsigned int port, void *address, unsigned long count );

extern
void io_outsw  ( unsigned int port, void *address, unsigned long count );

extern
void io_insl   ( unsigned int port, void *address, unsigned long count );

extern
void io_outsl  ( unsigned int port, void *address, unsigned long count );


extern
void io_memset ( unsigned int port, unsigned long value, unsigned long count );


extern
void io_memget ( unsigned int port, void *address, unsigned long count );

extern
void io_memput ( unsigned int port, void *address, unsigned long count );

#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_ASM_CORE_IO_H */
