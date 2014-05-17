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
; int test(unsigned char foo)
global test
test:
   push dword edx
   mov dword edx, [ esp + 0x8 ]
   mov dword eax,edx
   ;mov cl,24
   ;shr eax,cl
   pop dword edx 
   ret

; int test(unsigned char foo)
global test1
test1:
   mov dword eax, [ esp + 0x4 ]
   ;mov cl,24
   ;shr eax,cl
   ret

; int test(unsigned char foo)
; assert only bottom 8 bits are copied into al
; we can copy only 8 bits from the stack
; objects are on the stack in four bytes as
; stack is 32 bit, use our PARAM1-3 logic here.

global test2
test2:
    push dword edx
    mov dword edx, [ esp + 0x8 ]
    xor eax,eax
    mov byte al,dl
    pop  dword edx
    ret

%define HEAD   0x4
%define SECT   0x8
%define CYL1   0xC
%define CYL2   0x10
%define BUFF   0x14

; void pio_read(unsigned char drive_head, unsigned char sector, 
;               unsigned char cyl1, unsigned char cyl2, char *buffer);
;
; NOTE: - In C, the caller will push onto the stack and pop
;       the stack when done. The callee will not pop the stack
;       unless it needs local variables.
;       - drive_head contains a value for the drive (0 or 1) and
;         the head 0-15 based on 'get_drive_head' calculation.
;       - sector is the sector we want to address 0-63 or 0-255
;       - cyl1 is low and cyl2 is high part of the 16bit cylinder address
;         0-65535
;       - caller of pio_read receives a drive number and logical block address
;         converts it into chs and calls pio_read with drive+chs and a buffer
;         to read data into
;       - currently its setup to read one sector (512 byes) at a time
global pio_read
pio_read:
    xor eax,eax
    mov byte al, [esp + HEAD]
    mov byte al, [esp + SECT]
    mov byte al, [esp + CYL1]
    mov byte al, [esp + CYL2]
    mov dword eax, [esp + BUFF]
    ret

global bit_test1
bit_test1:
    mov byte al, [esp + 0x4]
    and al,1
    test al,al
    je .noerror
    mov byte al, [esp + 0x4]
    and al,128
    test al,al
    jne .noerror
    ; This is an error, bit 7 is 0 and bit 0 is 1.
    ; Read the error register and call the status handler.
    mov eax,1 ; Return 1 == error
    .noerror:
    mov eax,0 ; Return 0 == no error
    ret

extern status_check
global bit_test
bit_test:
    mov byte al, [esp + 0x4]
    test al,1 ; test performs a logical and; however, it does not modify operands
    jz .noerror
    test al,128
    jnz .noerror
    ; This is an error, bit 7(BSY) is 0 and bit 0(ERR) is 1.
    ; Read the error register and call the status handler.
    ; References pg 897 of Indispensable PC Hardware Book.
    ; Get the error register, push the stack, call the handler,
    ; output the error message, fixup the stack.
        push eax ; Push status_reg.
        ;mov dx,1f1h ; Read error port.
        ;in al,dx
        mov dword eax, 128
        push eax  ; Push error_reg.
        push dword 0 ; Indicate we were doing a read.
        call status_check
        add esp,12
        mov eax,1 ; Return 1 == error
        ret
    .noerror:
        ; Normal device operation here.
        mov eax,0 ; Return 0 == no error
        ret
