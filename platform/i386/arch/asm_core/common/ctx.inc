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
;		cpu_ctx.inc
;
;	@description:
;		Defines the Assembler macros for saving
;	and restoring the CPU Context.  This is used
;	for saving/restoring context for interrupt, exception, 
;	and system call handling.  It is referenced via
;	struct cpu_ctx, defined in the cpu_ctx.h C header file.
;	This structure is passed as an argument to several higher
;	level kernel functions found in:
;
;	exception.[ch]
;	signal.c
;
;	@author:
;		Roger G. Doss
;

; CTX_SAVE:
;	Stores the context of the CPU in accordance to struct
;	cpu_ctx, in cpu_ctx.h
%macro CTX_SAVE 0
	cld
	pushad
	push gs
	push fs
	push es
	push ds
%endmacro

; CTX_RESTORE:
;	Restores the context of the CPU, as saved by
;	CTX_SAVE macro.
%macro CTX_RESTORE 0
	pop ds
	pop es
	pop fs
	pop gs
	popad
%endmacro

;
; Interrupt/Exceptions where we switch
; privilege levels; the hardware push of registers
; automatically leaves the stack in the following
; layout:-
;
;	SS
;	ESP 
;	EFLAGS
;	CS
;	EIP
;
; In addition, with Exceptions that have an error code,
; the hardware will push the error code after the EIP
; register.
;
; we add to this the rest of the segment and general
; purpose registers, pushing in succession the 
; following:
;
;	EAX	( result of pushad )
;	ECX
;	EDX
;	EBX
;	ESP
;	EBP
;	ESI
;	EDI
;
;	GS
;	FS
;	ES
;	DS
;
;
; The result is a stack with the following offsets:
;
%define  __DS__		0x00
%define  __ES__		0x04
%define  __FS__		0x08
%define  __GS__		0x0C

%define  __EDI__	0x10
%define  __ESI__	0x14
%define  __EBP__	0x18
%define  __ESP__	0x1C

%define  __EBX__	0x20
%define  __EDX__	0x24
%define  __ECX__	0x28	
%define  __EAX__	0x2C

%define  __TMP__	0x30
%define  __EIP__	0x34
%define  __CS__		0x38
%define  __EFLAGS__	0x3C

%define  __SYS_ESP__	0x40
%define  __SS__		0x44

;
; EOF
;
