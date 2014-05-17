;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger George Doss. All Rights Reserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	@module:
;		proc.inc
;	@description:
;		Defines offsets into struct process.
;
;	@author:
;		Roger G. Doss
;	

;
;
;
extern current_process		; current process
extern previous_process		; previous process

;
;	The following are general defines for masking
;	flags in the EFLAGS register.
;
%define __CF_MASK__	0x00000001
%define __IF_MASK__	0x00000200
%define __NT_MASK__	0x00004000
%define __VM_MASK__	0x00020000

;
;	EOF
;
;
;	Offsets into the struct process
;	All are long words, except __PROC_TSS_SEG__
;	which is a word.
;
%define __PROC_STATE__		0x00
%define __PROC_COUNTER__	0x04
%define __PROC_PRIORITY__	0x08
%define __PROC_SIGNAL__		0x0C

%define __PROC_BLOCKED__	0x10
%define __PROC_RESCHEDULE__	0x14
%define __PROC_FLAGS__		0x18
%define __PROC_DBGREG6__	0x1C

%define __PROC_DBGREG7__	0x20
%define __PROC_TSS_SEG__    0x24
%define __PROC_EXIT_CODE__	0x28

;
; EOF
;
