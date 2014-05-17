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
;	read_tracks.inc
; @description
;	Access tracks from the disk
; 	this is a macro that calls read_track assembler routine
;	to do the actual BIOS call
; @author
;	Roger George Doss
;
%ifndef _READ_TRACKS_INC
%define _READ_TRACKS_INC

;
;	read_tracks:-
;
;	%1 cur_sect
;	%2 cur_head	
;	%3 cur_track
;	%4 es  segment address
;	%5 end segment
;	%6 total sectors per track
;	%7 boot drive
;
%macro read_tracks 7
	jmp %%real_read_tracks
		; messages
		_BOUNDARY_MSG
		_NEW_LINE

		; allocate temporary variables
		cur_sect  dw 0
		cur_head  dw 0
		cur_track dw 0

	%%real_read_tracks:
		mov word [cur_sect],%1	; setup sector
		mov word [cur_head],%2	; setup head
		mov word [cur_track],%3 ; setup track
		mov ax,%4		; setup load segment
		mov es,ax

		; test 64K boundary
		mov ax,es
		test ax,0x0FFF
		je %%1
		mov si,BOUNDARY_MSG
		call bprint
		call handle_err		; 64K boundary error

	%%1:	xor bx,bx
	%%2:	mov ax,es
		cmp ax,%5		; check if we are at end segment
		jb %%3
		jmp %%DONE		; RETURN

	%%3:	mov ax,%6
		sub ax,[cur_sect]	; subtract current read from total
					; sectors per track
		mov cx,ax
		shl cx,9		; multiply by 512 bytes to get number
					; of bytes left for read	
		add cx,bx
		jnc %%4
		je  %%4
		xor ax,ax
		sub ax,bx
		shr ax,9

	%%4:	push word [%7]
		push word [cur_sect]
		push word [cur_head]
		push word [cur_track]
		call track_read
		add  word sp,8
		mov  cx,ax
		add ax,[cur_sect]
		cmp ax,%6		; did we read all sectors
		jne %%6
		mov ax,1
		sub ax,[cur_head]
		jne %%5	
		inc word [cur_track]

	%%5:	mov [cur_head],ax
		xor ax,ax

	%%6:	mov [cur_sect],ax
		shl cx,9
		add bx,cx
		jnc %%2			; REPEAT READ
		mov ax,es
		add ax,0x1000
		mov es,ax
		xor bx,bx
		jmp %%2			; REPEAT READ
	%%DONE:
		mov si,NEW_LINE
		call bprint
	%endmacro

%endif ; _READ_TRACKS_INC
