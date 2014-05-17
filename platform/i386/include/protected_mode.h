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
/********************************************************
 * Copyright (C)  Roger George Doss. All Rights Reserved.
 ********************************************************
 *
 *  <platform/i386/protected_mode.h>
 *	
 *	prototypes for C protected mode routines
 *
 *******************************************************/
#ifndef _PLATFORM_I386_PROTECTED_MODE_H
#define _PLATFORM_I386_PROTECTED_MODE_H  1
#ifdef __cplusplus
 extern "C" {
#endif

extern
struct segment_descriptor *get_segment_descriptor(unsigned i);

extern
void protect_init( void );

extern
inline
void protect_init_codesegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,
                               unsigned char privaledge );

extern
inline
void protect_init_datasegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,   
                               unsigned char privaledge );

extern
inline
void protect_init_stacksegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,   
                               unsigned char privaledge );

extern
inline
void protect_init_tsssegment( register 
                               struct segment_descriptor *psd,
                               unsigned int base,
                               unsigned int limit,   
                               unsigned char privaledge );

extern
inline
void protect_init_ldtsegment( register
			      struct segment_descriptor *psd,
			      unsigned int base,
			      unsigned int limit,
			      unsigned char privaledge );

extern
inline
void protect_init_systemdesc( register
                              struct segment_descriptor *psd,
                              unsigned int base,
                              unsigned int limit );

extern
inline
void protect_init_intdesc  ( unsigned vector_number,
                             unsigned int base,
                             unsigned char privaledge );

extern
inline
void protect_init_trapdesc  ( unsigned vector_number,
			      unsigned int base,
			      unsigned char privaledge );


#ifdef __cplusplus
 }
#endif
#endif /* _PLATFORM_I386_PROTECTED_MODE_H */
