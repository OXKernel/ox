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
;	mem.inc
; @description
;	Generic memory utilities
; @author
;	Roger George Doss
;
%ifndef _MEM_INC
%define _MEM_INC

;
; memmove:-
; Move contents in memory to another memory
; location.
; ds:esi source
; ds:edi destination
; cx     length
extern memmove

;
; memzero:-
; Initializes memory to zero.
; ax   segment selector of memory
; edi  memory offset
; ecx  length
extern memzero

%endif ; _MEM_INC
