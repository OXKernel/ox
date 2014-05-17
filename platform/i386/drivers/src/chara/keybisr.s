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

extern keyboard_handler

global keyb_ISR
keyb_ISR:
    pusha
    push ds
    push es
    push ss
    push ss
    pop  ds
    pop  es
    call keyboard_handler
    pop es
    pop ds
    popa
    sti
    ;mov al,0x20
    ;out 0x20,al
    ;sti
    iretd
