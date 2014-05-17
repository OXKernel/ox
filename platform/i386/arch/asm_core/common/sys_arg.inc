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
;		sys_arg.inc
;
;	@description:
;		Defines how arguments are passed
;	to system calls.  This must be consistent
;	with how arguments are passed are passed
;	to system calls in the C header file:
;
;	syscall.h
;
;	NOTE: All possible arguments are passed,
;	      this allows the system calls to be written
;	      independently of the number of parameters
;	      they are to receive.  The C code generated
;	      by the compiler will access the approperiate
;	      region on the stack.
;
;	@author:
;		Roger G. Doss
;	

; 
; SYS_ARG_SET:
;
; 	This macro takes the saved registered, as placed
; 	on the stack by CTX_SAVE macro, and re-arranges
;	them in an order equal to the order used in
;	the system call C generation macros.
;
;	Thus, the register assignment can easily
;	be changed; however, one must be careful
;	to do the same for the C generation macros.
;
;	This works using three paramters, the stack
;	pointer, the register offset on the stack,
;	and the offset of the recently pushed value.
;
;	The stack is updated based on the offset of
;	the saved register as placed by CTX_SAVE and
;	the third paramter which is the offset on the stack
;	from where the recently pushed value exists.
;
%macro SYS_ARG_SET 0
	push dword [esp + __EBP__ + 0x00]	; Arg 6
	push dword [esp + __EDI__ + 0x04]	; Arg 5
	push dword [esp + __ESI__ + 0x08]	; Arg 4
	push dword [esp + __EDX__ + 0x0C]	; Arg 3
	push dword [esp + __ECX__ + 0x10]	; Arg 2
	push dword [esp + __EBX__ + 0x14]	; Arg 1
%endmacro

%macro SYS_ARG_UNSET 0
	add esp,24
%endmacro

;
; EOF
;
