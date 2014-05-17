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
;	util.inc
; @description
; 	utility functions
; 	
; 		- cpu_test
; 		- getkey
; 		- reboot
; 		- bprint
; @author
;	Roger G. Doss
;
%ifndef _UTIL_INC
%define _UTIL_INC

;
; cpu_test:
;	Tests if we are running on a x386 or above machine
;
extern cpu_test

;
; bprint:
;	print ds:si to screen
;
extern bprint

;
; hprint:
;	print 4 byte hexadecimal number
;
extern hprint

;
; getkey:
;	wait for key press
;
extern getkey

;
; reboot:
;	reboots the machine
;
extern reboot

%endif ; _UTIL_INC
