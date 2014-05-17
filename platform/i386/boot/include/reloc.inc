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
;	reloc.inc
; @description
;	macros to relocate an image, relocate an image 
; 	and setup a stack,
;	it is designed to work with a boot loader
;	but may work with other real mode code
; @author
;	Roger George Doss
;
%ifndef _RELOC_INC
%define _RELOC_INC

; Relocate the boot program
;	%1 -> initial     segment
;	%2 -> destination segment
;	%3 -> sizeof      segment
;	%4 -> organization address of code
%macro relocate 4
	mov ax,%1	; initial 	segment
	mov ds,ax
	mov ax,%2	; destination 	segment
	mov es,ax
	mov cx,%3	; sizeof 	segment
	sub si,si
	sub di,di
	rep
	movsw

	; After the relocation, we must do a far jump
	; to the address following this statement,
	; because the code is assumed to be organized at 
	; %4, we need
	; to subtract this from the address of .start
	jmp dword %2:(.start - %4)
	.start:

	; now correct the segments
	mov ax,cs	; code segment
	mov ds,ax
	mov es,ax
	mov ss,ax
	%endmacro

; Relocate the boot program
;	%1 -> initial     segment
;	%2 -> destination segment
;	%3 -> sizeof      segment
;	%4 -> organization address of code
;	%5 -> address of stack
%macro relocate_setstack 5
	relocate %1,%2,%3,%4
	mov sp,%5	; setup stack
	%endmacro

%endif ; _RELOC_INC
