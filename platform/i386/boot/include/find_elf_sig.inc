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
;Copyright (C) Roger G. Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;@module:
;	find_elf_sig.inc
; @description:
;	Scan memory from 0x10000 ( 64K ) to 
;	0x100000 ( 1meg) looking for ELF signature
; 	if it is found, we set up eax with the
;	address, otherwise we halt, this is 
;	meant to be used for debugging the boot
;	process.
;	NOTE: One may use hprint to print out
;	      the address in HEX, by using:-
;
;	      push eax
;	      call hprint
;	      add sp,4
;
; @author:
;	Roger G. Doss
;
%ifndef _FIND_ELF_SIG
%define _FIND_ELF_SIG

%macro find_elf_sig 0
	xor eax,0x10000
	jmp redo
	error db 'incorrectly loaded file',13,10,0

	redo:
		cmp dword [eax],0x464C457F
		je %%1

		add eax,0x4
		cmp eax,0x100000
		je %%2
		jmp redo
	%%2:
		mov si,error
		call bprint
		cli
		hlt
	%%1:
	%endmacro

%endif ; _FIND_ELF_SIG
