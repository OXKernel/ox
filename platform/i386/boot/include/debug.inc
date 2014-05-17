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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger George Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @module
;	debug.inc
; @description
;	debugging facilities
;
; @author
;	Roger George Doss
;
%ifndef _DEBUG_INC
%define _DEBUG_INC

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;		Debugging Macros.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro  DEBUG 0
	pusha
	mov ax,'O'+0x0E00	; VGA ram address
	mov bx, 7		; ASCII flags
	int 0x10
	popa
	%endmacro
	
%macro DEBUG1 0
	pusha
	mov ax, 'X'+0x0E00
	mov bx, 7
	int 0x10
	popa
	%endmacro

;
; print_reg:-
;
; Prints in hex the contents of
; a register.
;
%macro print_reg 1
	pusha
	push %1
        call hprint
	add sp,4	
	popa
	%endmacro

;
; stop:-
;  
; Disable interrupts and halt the
; cpu.
;
%macro stop 0
	cli
	hlt
	%endmacro 

%endif ; _DEBUG_INC
