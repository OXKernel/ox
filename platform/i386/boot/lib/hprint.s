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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copyright (C) Roger G. Doss. All Rights Reserved.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; @module: 
;	hprint.s
; @description:
;	implementation of hprint
; @author: 
;	Roger G. Doss
;
%include "include/common.inc"

[BITS 16]

;
; hprint:
;	Print out a hexadecimal number for debugging.
;
%define PARAM1 0x4
ENTRY hprint
	push bp
	mov bp,sp
	push edx
	push cx	
		mov cx,8
		mov edx,[bp + PARAM1]
		.print:
			rol	edx, 0x4
			mov	ax, 0xE0F
			and	al, dl
			add	al, 0x90
			daa
			adc	al, 0x40
			daa
			int	0x10
			loop .print
	pop cx
	pop edx
	leave
	ret
