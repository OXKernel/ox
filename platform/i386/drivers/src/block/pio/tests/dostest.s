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
[BITS 16]
;
;  Code originally developed by qark retrieved from
;  http://forum.osdev.org/viewtopic.php?t=12268
;  Adapted for use in C by Dr. Roger G. Doss, PhD
;
;  Technical Information on the ports:
;      Port    Read/Write   Misc
;     ------  ------------ -------------------------------------------------
;       1f0       r/w       data register, the bytes are written/read here
;       1f1       r         error register  (look these values up yourself)
;       1f2       r/w       sector count, how many sectors to read/write
;       1f3       r/w       sector number, the actual sector wanted
;       1f4       r/w       cylinder low, cylinders is 0-1024
;       1f5       r/w       cylinder high, this makes up the rest of the 1024
;       1f6       r/w       drive/head
;                              bit 7 = 1
;                              bit 6 = 0
;                              bit 5 = 1
;                              bit 4 = 0  drive 0 select
;                                    = 1  drive 1 select
;                              bit 3-0    head select bits
;       1f7       r         status register
;                              bit 7 = 1  controller is executing a command
;                              bit 6 = 1  drive is ready
;                              bit 5 = 1  write fault
;                              bit 4 = 1  seek complete
;                              bit 3 = 1  sector buffer requires servicing
;                              bit 2 = 1  disk data read corrected
;                              bit 1 = 1  index - set to 1 each revolution
;                              bit 0 = 1  previous command ended in an error
;       1f7       w         command register
;                            commands:
;                              50h format track
;                              20h read sectors with retry
;                              21h read sectors without retry
;                              22h read long with retry
;                              23h read long without retry
;                              30h write sectors with retry
;                              31h write sectors without retry
;                              32h write long with retry
;                              33h write long without retry
;
;  Most of these should work on even non-IDE hard disks.
;

; void pio_read(unsigned char drive_head, unsigned char sector, 
;               unsigned char cyl1, unsigned char cyl2, char *buffer);
;
; NOTE: - In C, the caller will push onto the stack and pop
;         the stack when done. The callee will not pop the stack
;         unless it needs local variables.
;       - drive_head contains a value for the drive (0 or 1) and
;         the head 0-15 based on 'get_drive_head' calculation.
;       - sector is the sector we want to address 0-63 or 0-255
;       - cyl1 is low and cyl2 is high part of the 16bit cylinder address
;         0-65535
;       - caller of pio_read receives a drive number and logical block address
;         converts it into chs and calls pio_read with drive+chs and a buffer
;         to read data into
;       - currently its setup to read one sector (512 byes) at a time
%define HEAD   0x4
%define SECT   0x6
%define CYL1   0x8
%define CYL2   0xA
%define BUFF   0xC

section .data
readmsg: db      "The buffers match.  Hard disk read using ports.",13,10,"$"
failmsg: db      "The buffers do not match.",13,10,"$"
buffer:  times 512 db 0 ; can also use "resb 512" but that will reserve 512 bytes uninitialized
buffer2: times 512 db 0

section .text
global pio_read
pio_read:
   push word bp
   mov bp,sp
   xor ax,ax
   mov     dx,1f6h         ;Drive and head port
   mov byte al,[bp + HEAD] ;Drive , head  PARAM
   out     dx,al

   mov     dx,1f2h         ;Sector count port
   mov     al,1            ;Read one sector
   out     dx,al

   mov     dx,1f3h         ;Sector number port
   mov byte al,[bp + SECT];Read sector PARAM
   out     dx,al

   mov     dx,1f4h         ;Cylinder low port
   mov byte al,[bp + CYL1];Cylinder PARAM
   out     dx,al

   mov     dx,1f5h         ;Cylinder high port
   mov byte al,[bp + CYL2];The rest of the cylinder - PARAM
   out     dx,al

   mov     dx,1f7h         ;Command port
   mov     al,20h          ;Read with retry.
   out     dx,al
.still_going:
   in      al,dx
   test    al,8            ;This means the sector buffer requires
                           ;servicing.
   jz      .still_going     ;Don't continue until the sector buffer
                           ;is ready.

   ; This code actuall does the data transfer into the buffer.
   ; PARAM pass in the buffer on the stack.
   ; TODO - Double check we use the edi/esi register here look at our io.s code.
   ;      - Try to see if we can do some error handling somewhere, otherwise return success
   ;        to dev.c.
   ;      - Compile/link the code.
   ;      - Review the TODO in the backup/1 code in pioread.s
   mov     cx,512/2        ;One sector /2
   mov word di,[bp + BUFF]
   mov     dx,1f0h         ;Data port - data comes in and out of here.
   rep     insw
   xor ax,ax

   ; -- *BIOS* -- compare

   mov     ax,201h         ;Read using int13h then compare buffers.
   mov     dx,80h
   mov     cx,1
   mov     bx,buffer2
   int     13h

   mov     cx,512
   mov     si,[bp + BUFF]
   mov     di,buffer2
   repe    cmpsb
   jne     failure
   mov     ah,9
   mov     dx,readmsg
   int     21h
   jmp     good_exit
failure:
   mov     ah,9
   mov     dx,failmsg
   int     21h
good_exit:
   mov     ax,4c00h        ;Exit the program
   int     21h
   pop word bp
   ret

global main
main:
   mov ax, buffer ; buffer
   push word ax
   mov ax, 0 ; cylinder 0 high
   push word ax
   mov ax, 0 ; cylinder 0 low
   push word ax
   mov ax, 1 ; sector one
   push word ax
   mov ax, 0a0h ; drive_head
   push word ax
   call pio_read
   xor ax,ax
   add sp, 10
   ret
