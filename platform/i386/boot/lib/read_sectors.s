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
;	read_sectors.s
; @description
;   Read sectors from disk.
; @author
;	Roger George Doss
;
%include "include/common.inc"

[BITS 16]

;
; read_sectors:-
; Input:
;	EAX = LBN
;	DI  = sector count
;	ES = segment
;
;   _S2_NR_SECT 0x12 (18 sectors per track in s2.s)
;   _S2_NR_HEAD 0x2  (2 heads in s2.s)
;   boot_drive (0 if floppy from caller in s2.s)
;
; Output:
;	BL = low byte of ES
;	EBX high half cleared
;	DL = 0x80
;	EDX high half cleared
;	ESI = 0
ENTRY read_sectors
	pusha

	cdq			;edx = 0
	mov	ebx, _S2_NR_SECT
	div	ebx		;EAX=track ;EDX=sector-1
	mov	cx, dx		;CL=sector-1 ;CH=0
	inc	cx		;CL=Sector number
	xor	dx, dx
	mov	bl, _S2_NR_HEAD
	div	ebx

	mov	dh, dl		;Head
	mov	dl, [boot_drive]	;Drive 0
	xchg	ch, al		;CH=Low 8 bits of cylinder number; AL=0
	shr	ax, 2		;AL[6:7]=High two bits of cylinder; AH=0
	or	cl, al		;CX = Cylinder and sector
	mov	ax, di		;AX = Maximum sectors to xfer
retry:	mov	ah, 2		;Read
	xor	bx, bx
	int	13h
	jc retry

	popa
	ret

;
;
; EOF
;
