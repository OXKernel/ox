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
;	sector_read.s
; @description
;	implementation of sector_read
; @author
;	Roger George Doss
;
%include "include/common.inc"

[BITS 16]

;
; sector_read:-
;	Use BIOS int 0x13 to read sectors
;	into memory.
;
;	Global Parameters:-
;		nr_sectors
;		nr_heads
;		boot_drive
;
;	Input:
;		eax = LBN
;		di  = sector count
;		es  = segment
;
;	Ouput:
;		bl  = low order byte of cs
;		ebx = high order bits cleared
;		edx = high order bits cleared
;		dl  = drive number
;		esi = 0
ENTRY sector_read
	pusha				; save general purpose registers
	cdq				; sign extend eax into edx:eax
	movzx ebx,byte [nr_sectors]	; load number of sectors
	div ebx				; edx:eax/ebx, quotient = eax, remainder = edx
	mov cx,dx			; mov remainder into cx
	inc cx				; increment by one
	xor dx,dx			; clear dx
	mov bl,[nr_heads]		; load number of heads
	div ebx				; edx:eax/ebx, quotient = eax, remainder = edx

	mov dh,dl			; set-up head
	mov dl,[boot_drive]		; set-up drive to read from
	xchg ch,al			; ch = low 8 bits of cylinder number
	shr ax,2			; al = high order bits of cylinder number
	or cl,al			; cx = cylinder and sector number
	mov ax,di			; ax = number of bytes to read
.retry: mov ah,2			; tell BIOS to read
	xor bx,bx			; clear bx
	int 0x13			; request disk read from BIOS
	jc  .retry			; retry if we failed
	popa				; restore general purpose registers
	ret

;
; EOF
;
