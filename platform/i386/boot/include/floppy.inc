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
;	floppy.inc
; @description
;	Access the floppy disk using BIOS
; @author
;	Roger George Doss
;
%ifndef _FLOPPY_INC
%define _FLOPPY_INC

;
; sector_read:-
;	Use BIOS int 0x13 to read sectors
;	into memory.
;
;	Input:
;		eax = LBN
;		di  = sector count
;		es  = segment
;	Ouput:
;		bl  = low order byte of cs
;		ebx = high order bits cleared
;		edx = high order bits cleared
;		dl  = drive number
;		esi = 0
extern sector_read

;
; turn_off_floppy:-
;	Turn off the floppy drive.
;
extern turn_off_floppy

%endif ; _FLOPPY_INC
